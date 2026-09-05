//2
#include <Arduino.h>

static const int PIN_ADC   = 4;
static const int N_AVG     = 16;
static const int N_BINS    = 64;
static const int BIN_SHIFT = 6;
static const int MIN_HITS  = 4;

typedef struct {
  uint32_t n;
  uint32_t sum_raw;
  uint32_t sum_mv;
} bin_t;

static bin_t bins[N_BINS];

static float ep_a = 0.0f, ep_b = 0.0f;
static bool  sweeping = false;
static uint32_t t_status = 0;

// ---------------------------------------------------------
static void sample_once(void)
{
  uint32_t sr = 0, sm = 0;

  for (int i = 0; i < N_AVG; i++) {
    sr += (uint32_t)analogRead(PIN_ADC);
    sm += (uint32_t)analogReadMilliVolts(PIN_ADC);
  }

  uint32_t raw = sr / N_AVG;
  uint32_t mv  = sm / N_AVG;

  int b = (int)(raw >> BIN_SHIFT);
  if (b < 0) b = 0;
  if (b >= N_BINS) b = N_BINS - 1;

  bins[b].n++;
  bins[b].sum_raw += raw;
  bins[b].sum_mv  += mv;
}

static bool bin_filled(int i) { return bins[i].n >= MIN_HITS; }
static float bin_raw(int i)   { return (float)bins[i].sum_raw / bins[i].n; }
static float bin_mv(int i)    { return (float)bins[i].sum_mv  / bins[i].n; }

static int coverage(void)
{
  int c = 0;
  for (int i = 0; i < N_BINS; i++) if (bin_filled(i)) c++;
  return c;
}

static void show_coverage(void)
{
  Serial.print("покриття [");
  for (int i = 0; i < N_BINS; i++) Serial.print(bin_filled(i) ? '#' : '.');
  Serial.printf("] %d/%d\n", coverage(), N_BINS);
}


static bool build_endpoint_line(void)
{
  int lo = -1, hi = -1;

  for (int i = 0; i < N_BINS; i++) {
    if (!bin_filled(i)) continue;
    if (lo < 0) lo = i;
    hi = i;
  }
  if (lo < 0 || hi == lo) return false;

  float x0 = bin_raw(lo), y0 = bin_mv(lo);
  float x1 = bin_raw(hi), y1 = bin_mv(hi);

  ep_a = (y1 - y0) / (x1 - x0);
  ep_b = y0 - ep_a * x0;
  return true;
}


static bool build_lsq_line(float *a, float *b)
{
  double sx = 0, sy = 0, sxx = 0, sxy = 0;
  int n = 0;

  for (int i = 0; i < N_BINS; i++) {
    if (!bin_filled(i)) continue;
    double x = bin_raw(i), y = bin_mv(i);
    sx += x; sy += y; sxx += x * x; sxy += x * y;
    n++;
  }
  if (n < 2) return false;

  double det = (double)n * sxx - sx * sx;
  if (det == 0.0) return false;

  *a = (float)(((double)n * sxy - sx * sy) / det);
  *b = (float)((sy - (double)(*a) * sx) / n);
  return true;
}


static void report(void)
{
  if (coverage() < 8) { Serial.println("[!] замало даних, покрути ще"); return; }
  if (!build_endpoint_line()) { Serial.println("[!] не вдалось побудувати пряму"); return; }

  float lsq_a, lsq_b;
  bool have_lsq = build_lsq_line(&lsq_a, &lsq_b);

  Serial.println();
  Serial.printf("Пряма по крайніх точках:  V = %.5f * raw %+.2f\n", ep_a, ep_b);
  if (have_lsq)
    Serial.printf("Пряма найменших квадратів: V = %.5f * raw %+.2f\n", lsq_a, lsq_b);
  Serial.println();
  Serial.println("raw\tзавод\tпряма\tdelta\tdelta\tdelta");
  Serial.println("сер.\tмВ\tмВ\tмВ\tLSB\t%");

  float worst = 0.0f;   int worst_raw = 0;
  float worst_l = 0.0f;                      // для LSQ
  int   ok_lo = -1, ok_hi = -1;              // зона, де |delta| <= 10 мВ

  for (int i = 0; i < N_BINS; i++) {
    if (!bin_filled(i)) continue;

    float x = bin_raw(i);
    float y = bin_mv(i);
    float line = ep_a * x + ep_b;
    float d = y - line;                       // ось вона, нелінійність

    float d_lsb = (ep_a != 0.0f) ? d / ep_a : 0.0f;
    float d_pct = (y > 1.0f) ? (d / y) * 100.0f : 0.0f;

    Serial.printf("%6.0f\t%6.0f\t%7.1f\t%+6.1f\t%+6.1f\t%+6.2f\n",
                  x, y, line, d, d_lsb, d_pct);

    if (fabsf(d) > fabsf(worst)) { worst = d; worst_raw = (int)x; }

    if (fabsf(d) <= 10.0f) {
      if (ok_lo < 0) ok_lo = (int)x;
      ok_hi = (int)x;
    }

    if (have_lsq) {
      float dl = y - (lsq_a * x + lsq_b);
      if (fabsf(dl) > fabsf(worst_l)) worst_l = dl;
    }
  }

  Serial.println();
  Serial.printf("Максимальне відхилення: %+.1f мВ (%.1f LSB) при raw = %d\n",
                worst, worst / ep_a, worst_raw);
  if (have_lsq)
    Serial.printf("Те саме для прямої МНК:  %+.1f мВ  -> виграш %.0f%%\n",
                  worst_l, (1.0f - fabsf(worst_l) / fabsf(worst)) * 100.0f);

  if (ok_lo >= 0)
    Serial.printf("Зона, де |delta| <= 10 мВ: raw %d ... %d\n", ok_lo, ok_hi);
  else
    Serial.println("Зони з |delta| <= 10 мВ немає взагалі");
  Serial.println();
}

static void teleplot(void)
{
  if (!build_endpoint_line()) return;

  for (int i = 0; i < N_BINS; i++) {
    if (!bin_filled(i)) continue;
    float x = bin_raw(i);
    float d = bin_mv(i) - (ep_a * x + ep_b);
    Serial.printf(">inl:%.0f:%.2f|xy\n", x, d);
  }
  Serial.println("[Teleplot] крива відправлена");
}

// ---------------------------------------------------------
static void printHelp(void)
{
  Serial.println();
  Serial.println("=== КРИВА ПОХИБКИ ПРЯМОЇ (endpoint-fit INL) ===");
  Serial.println("  s - старт/стоп свіпу (крути потенціометр повільно, туди-сюди)");
  Serial.println("  p - таблиця відхилень");
  Serial.println("  g - крива у Teleplot");
  Serial.println("  c - очистити накопичене");
  Serial.println("  h - довідка");
  Serial.println();
}

static bool readLine(String &out)
{
  if (!Serial.available()) return false;
  out = Serial.readStringUntil('\n');
  out.trim();
  return out.length() > 0;
}

void setup()
{
  Serial.begin(115200);
  Serial.setTimeout(50);
  analogReadResolution(12);
  analogSetPinAttenuation(PIN_ADC, ADC_11db);
  memset(bins, 0, sizeof(bins));
  delay(500);
  printHelp();
}

void loop()
{
  String line;
  if (readLine(line)) {
    char c = line[0];
    if (c == 's') {
      sweeping = !sweeping;
      Serial.println(sweeping ? "свіп ЗАПУЩЕНО" : "свіп зупинено");
    }
    else if (c == 'p') report();
    else if (c == 'g') teleplot();
    else if (c == 'c') { memset(bins, 0, sizeof(bins)); Serial.println("очищено"); }
    else printHelp();
  }

  if (sweeping) {
    sample_once();

    uint32_t now = millis();
    if (now - t_status >= 700) {
      t_status = now;
      show_coverage();
    }
  } else {
    delay(20);
  }
}