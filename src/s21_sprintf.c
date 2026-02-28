#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>

#include "s21_string.h"

/*
Идея:
 - читает format
 - Читает аргументы из ... через va_list
 - Записывает результат в str
 - Ставит в конце '\0'
 - Возвращает количество записанных символов без '\0'

Поддержка:
 - flags: '-', '+', ' ', '0', '#'
 - width: number, '*'
 - precision: .number, '.*'
 - length: h, l, L
 - spec: c, s, d, i, u, o, x, X, p, n, %, f, e, E, g, G
*/

// -------------------- мини-утилиты без <ctype.h> --------------------

static int s21_is_digit_char(char c) { return (c >= '0' && c <= '9'); }

static long double s21_fabsl(long double x) { return (x < 0) ? -x : x; }

static long double s21_pow10_ld(int exp) {
  long double p = 1.0L;
  if (exp >= 0) {
    for (int i = 0; i < exp; i++) p *= 10.0L;
  } else {
    for (int i = 0; i < -exp; i++) p /= 10.0L;
  }
  return p;
}

static int s21_is_nan_ld(long double v) { return isnan((double)v) ? 1 : 0; }
static int s21_is_inf_ld(long double v) { return isinf((double)v) ? 1 : 0; }

// -------------------- структура спецификатора --------------------

typedef struct {
  int flag_minus;
  int flag_plus;
  int flag_space;
  int flag_zero;
  int flag_hash;

  int width;      // -1 = не задан
  int precision;  // -1 = не задан

  char length;  // 0, 'h', 'l', 'L'
  char spec;
} s21_fmt_t;

static void s21_fmt_init(s21_fmt_t *fs) {
  fs->flag_minus = 0;
  fs->flag_plus = 0;
  fs->flag_space = 0;
  fs->flag_zero = 0;
  fs->flag_hash = 0;
  fs->width = -1;
  fs->precision = -1;
  fs->length = 0;
  fs->spec = 0;
}

static void s21_parse_flags(const char **pf, s21_fmt_t *fs) {
  int cont = 1;
  while (cont && **pf) {
    switch (**pf) {
      case '-':
        fs->flag_minus = 1;
        (*pf)++;
        break;
      case '+':
        fs->flag_plus = 1;
        (*pf)++;
        break;
      case ' ':
        fs->flag_space = 1;
        (*pf)++;
        break;
      case '0':
        fs->flag_zero = 1;
        (*pf)++;
        break;
      case '#':
        fs->flag_hash = 1;
        (*pf)++;
        break;
      default:
        cont = 0;
        break;
    }
  }
}

static int s21_parse_int(const char **pf) {
  int v = 0;
  while (**pf && s21_is_digit_char(**pf)) {
    v = v * 10 + (int)(**pf - '0');
    (*pf)++;
  }
  return v;
}

static void s21_parse_width(const char **pf, s21_fmt_t *fs, va_list *ap) {
  if (**pf == '*') {
    (*pf)++;
    int w = va_arg(*ap, int); //извлечение ширины из списка аргументов
    if (w < 0) { //отрицательная ширина - выравнивание влево + ширина поля
      fs->flag_minus = 1;
      w = -w;
    }
    fs->width = w;
  } else if (**pf && s21_is_digit_char(**pf)) {
    fs->width = s21_parse_int(pf);
  }
}

static void s21_parse_precision(const char **pf, s21_fmt_t *fs, va_list *ap) {
  if (**pf != '.') return;
  (*pf)++;

  if (**pf == '*') {
    (*pf)++;
    int p = va_arg(*ap, int);
    fs->precision = (p < 0) ? -1 : p;
  } else {
    if (**pf && s21_is_digit_char(**pf)) {
      fs->precision = s21_parse_int(pf);
    } else {
      fs->precision = 0;
    }
  }
}

static void s21_parse_length(const char **pf, s21_fmt_t *fs) {
  if (**pf == 'h' || **pf == 'l' || **pf == 'L') {
    fs->length = **pf;
    (*pf)++;
  }
}

// -------------------- запись в выходной буфер --------------------

static void s21_out_repeat(char **out, int *written, char ch, int n) {
  for (int i = 0; i < n; i++) {
    **out = ch;
    (*out)++;
    (*written)++;
  }
}

static void s21_out_mem(char **out, int *written, const char *src, int n) {
  for (int i = 0; i < n; i++) {
    **out = src[i];
    (*out)++;
    (*written)++;
  }
}

//возвращает длину строки или limit
static int s21_cstrlen_limit(const char *s, int limit) {
  int n = 0;
  if (!s) return 0;
  while (s[n] != '\0' && (limit < 0 || n < limit)) n++;
  return n;
}

static void s21_emit_with_width(char **out, int *written, const s21_fmt_t *fs,
                                const char *tmp, int tmp_len, int zero_ok) {
  int width = (fs->width < 0) ? 0 : fs->width;
  int pad = (width > tmp_len) ? (width - tmp_len) : 0; //сколько символов нужно заполнить до заданной ширины
  //если установлен флаг заполнения нулями и не выбрано выравнивание слева, то заполняем нулями, иначе - пробелами
  char pad_ch = (zero_ok && fs->flag_zero && !fs->flag_minus) ? '0' : ' ';

  if (!fs->flag_minus) {
    //запись в буфер числа со знаком
    if (pad_ch == '0' && tmp_len > 0 &&
        (tmp[0] == '+' || tmp[0] == '-' || tmp[0] == ' ')) {
      s21_out_mem(out, written, tmp, 1); //запись знака в буфер
      s21_out_repeat(out, written, '0', pad); //запись дополняющих нулей
      s21_out_mem(out, written, tmp + 1, tmp_len - 1); //запись самого числа
      return;
    }
    //запись числа без знака
    s21_out_repeat(out, written, pad_ch, pad); 
    s21_out_mem(out, written, tmp, tmp_len); 
  } else {
    s21_out_mem(out, written, tmp, tmp_len);
    s21_out_repeat(out, written, ' ', pad);
  }
}

// -------------------- int/uint конвертация --------------------

static int s21_utoa_base_rev(char *buf, unsigned long long v, int base,
                             int upper) {
  //алфавиты доступных символов
  const char *dl = "0123456789abcdef";
  const char *du = "0123456789ABCDEF";
  const char *d = upper ? du : dl;

  int len = 0;
  if (v == 0ULL) {
    buf[len++] = '0';
    return len;
  }
  while (v != 0ULL) {
    unsigned dig = (unsigned)(v % (unsigned long long)base); //получение разряда числа
    buf[len++] = d[dig]; //поиск разряда в алфавите доступных символов и запись его в буфер
    v /= (unsigned long long)base; //переход к следующему разряду(более старшему)
  }
  return len;
}

static int s21_format_int(char *tmp, const s21_fmt_t *fs, long long sval,
                          unsigned long long uval, int is_signed, int base,
                          int upper, int is_pointer) {
  int precission = fs->precision;
  int precission_set = (precission >= 0);

  char sign_ch = 0;
  unsigned long long mag = 0ULL; //модуль числа
  if (is_signed) {
    if (sval < 0) {
      sign_ch = '-';
      mag = (unsigned long long)(-(sval + 1)) + 1ULL; //для корректной обработки -2^63, иначе будет переполнение
    } else {
      mag = (unsigned long long)sval;
      if (fs->flag_plus)
        sign_ch = '+';
      else if (fs->flag_space)
        sign_ch = ' ';
    }
  } else {
    mag = uval;
  }

  int value_is_zero = (mag == 0ULL);

  char digits_rev[128]; //массив символов для хранения считанных данных
  int digits_len = 0; //количество выводимых символов

  int digits_empty = 0; //признак пустого значения

  //условие precission==0 строже чем precission_set, по идее можно оставить только precission==0
  //если передали не указатель с точностью 0(вывод всех символов) и само значение нулевое,
  //то нужно вывести пустое значение за исключением случая восьмеричной системы, когда 0 выведется
  if (!is_pointer && precission_set && precission == 0 && value_is_zero) {
    if (fs->spec == 'o' && fs->flag_hash) {
      digits_rev[0] = '0';
      digits_len = 1;
    } else {
      digits_empty = 1;
    }
  }

  if (!digits_empty)
    digits_len = s21_utoa_base_rev(digits_rev, mag, base, upper);

  char prefix[3]; //префиксы для 8-чной и 16-чной систем
  int prefix_len = 0;
  //для указателей выводим адрес в 16-чной системе
  if (is_pointer) {
    prefix[prefix_len++] = '0';
    prefix[prefix_len++] = 'x';
  } else if (fs->flag_hash) {
    if ((fs->spec == 'x' || fs->spec == 'X') && !value_is_zero) {
      prefix[prefix_len++] = '0';
      prefix[prefix_len++] = (fs->spec == 'X') ? 'X' : 'x';
    } else if (fs->spec == 'o') {
      if (!(digits_len > 0 && digits_rev[digits_len - 1] == '0'))
        prefix[prefix_len++] = '0';
    }
  }

  int need_zeros = 0;
  if (precission_set && precission > digits_len)
    need_zeros = precission - digits_len; //количество дополнительных нулей 

  int width = (fs->width < 0) ? 0 : fs->width;
  int zero_pad = 0;
  if (!precission_set && fs->flag_zero && !fs->flag_minus) {
    //общая длина выводимой строки складывается из знака(если есть), префикса, дополнительных(ведущих) нулей и разрядов самого числа
    int core_len = digits_len + need_zeros + prefix_len + (sign_ch ? 1 : 0);
    //ширина поля больше чем требуемая длина выводимого значения - дополняем нулями справа(их будет zero_pad штук)
    if (width > core_len) zero_pad = width - core_len; 
  }

  //заполняем выходной буфер
  int pos = 0;
  if (sign_ch) tmp[pos++] = sign_ch;
  for (int i = 0; i < prefix_len; i++) tmp[pos++] = prefix[i];
  for (int i = 0; i < zero_pad; i++) tmp[pos++] = '0'; //почему все нули выводятся слева, по идее need_zeros дожно быть слева, а zero_pad - справа
  for (int i = 0; i < need_zeros; i++) tmp[pos++] = '0';

  //записываем разряды в привычном виде - от старшего к младшему(получали сначала младший, а потом старший)
  for (int i = digits_len - 1; i >= 0; i--) tmp[pos++] = digits_rev[i];

  tmp[pos] = '\0';
  return pos;
}

// -------------------- float форматирование --------------------

static void s21_upper_str(char *s) {
  for (int i = 0; s[i] != '\0'; i++) {
    if (s[i] >= 'a' && s[i] <= 'z') {
      s[i] = (char)(s[i] - ('a' - 'A'));
    }
  }
}

static int s21_format_nan_inf(char *tmp, const s21_fmt_t *fs, long double v) {
  int pos = 0;
  if (signbit((double)v)) //проверка старшего разряда, зарезервинного под знак
    tmp[pos++] = '-';
  else if (fs->flag_plus)
    tmp[pos++] = '+';
  else if (fs->flag_space)
    tmp[pos++] = ' ';

  //для неопределенного значения выведем "nan", для бесконечности "inf"
  if (s21_is_nan_ld(v)) {
    tmp[pos++] = 'n';
    tmp[pos++] = 'a';
    tmp[pos++] = 'n';
  } else {
    tmp[pos++] = 'i';
    tmp[pos++] = 'n';
    tmp[pos++] = 'f';
  }

  tmp[pos] = '\0';
  //для спецификаторов Е, G выводим в верхнем регистре
  if (fs->spec == 'E' || fs->spec == 'G') s21_upper_str(tmp);
  return pos;
}

static int s21_format_f(char *tmp, const s21_fmt_t *fs, long double v,
                        int prec) {
  if (s21_is_nan_ld(v) || s21_is_inf_ld(v))
    return s21_format_nan_inf(tmp, fs, v);
  if (prec < 0) prec = 6; //точность по умолчанию

  int pos = 0;
  int neg = (v < 0);
  long double av = s21_fabsl(v);

  if (neg)
    tmp[pos++] = '-';
  else if (fs->flag_plus)
    tmp[pos++] = '+';
  else if (fs->flag_space)
    tmp[pos++] = ' ';

  long double scale = s21_pow10_ld(prec); //10^prec
  long double rounded = floorl(av * scale + 0.5L); //округление снизу

  unsigned long long int_part = (unsigned long long)(rounded / scale); //получаем целую часть числа
  unsigned long long frac_scaled =
      (unsigned long long)(rounded - (long double)int_part * scale); //получаем дробную часть числа

  char int_rev[128];
  int int_len = s21_utoa_base_rev(int_rev, int_part, 10, 0);
  for (int i = int_len - 1; i >= 0; i--) tmp[pos++] = int_rev[i]; //записываем в буфер целую часть

  if (prec > 0 || fs->flag_hash) {
    tmp[pos++] = '.';
    //вывод в буфер дробных разрядов от старшего к младшему
    for (int i = prec - 1; i >= 0; i--) {
      unsigned long long div = (unsigned long long)s21_pow10_ld(i);
      unsigned digit = (unsigned)(frac_scaled / div);
      tmp[pos++] = (char)('0' + digit);
      frac_scaled %= div;
    }
  }

  tmp[pos] = '\0';
  return pos;
}

static int s21_format_e(char *tmp, const s21_fmt_t *fs, long double v, int prec,
                        int upper) {
  if (s21_is_nan_ld(v) || s21_is_inf_ld(v))
    return s21_format_nan_inf(tmp, fs, v);
  if (prec < 0) prec = 6;

  int pos = 0;
  int neg = (v < 0);
  long double av = s21_fabsl(v);

  if (neg)
    tmp[pos++] = '-';
  else if (fs->flag_plus)
    tmp[pos++] = '+';
  else if (fs->flag_space)
    tmp[pos++] = ' ';

  //должна быть одна цифра до запятой в [1,10) для v!=0 и 0 для v==0
  int exp10 = 0;
  if (av != 0.0L) {
    while (av >= 10.0L) {
      av /= 10.0L;
      exp10++;
    }
    while (av < 1.0L) {
      av *= 10.0L;
      exp10--;
    }
  }

  long double scale = s21_pow10_ld(prec);
  long double rounded = floorl(av * scale + 0.5L) / scale;
  //если после округления дробной части получили 10, то увеличиваем експоненту на 1, а дробную часть делим на 10
  if (rounded >= 10.0L) {
    rounded /= 10.0L;
    exp10++;
  }

  int first = (int)floorl(rounded);
  tmp[pos++] = (char)('0' + first); //записываем целую часть в буфер

  long double frac = rounded - (long double)first;

  //записываем в буфер дробную часть
  if (prec > 0 || fs->flag_hash) {
    tmp[pos++] = '.';
    unsigned long long frac_scaled =
        (unsigned long long)floorl(frac * scale + 0.5L);
    for (int i = prec - 1; i >= 0; i--) {
      unsigned long long div = (unsigned long long)s21_pow10_ld(i);
      unsigned digit = (unsigned)(frac_scaled / div);
      tmp[pos++] = (char)('0' + digit);
      frac_scaled %= div;
    }
  }

  tmp[pos++] = upper ? 'E' : 'e';
  tmp[pos++] = (exp10 < 0) ? '-' : '+';

  //вычисление разрядов экспоненты -  сотни, десятки и единицы
  int eabs = (exp10 < 0) ? -exp10 : exp10;
  int hundreds = eabs / 100;
  int tens = (eabs / 10) % 10;
  int ones = eabs % 10;

  //сотни выведем только если экспонента больше 99
  if (hundreds > 0) tmp[pos++] = (char)('0' + hundreds);
  tmp[pos++] = (char)('0' + tens);
  tmp[pos++] = (char)('0' + ones);

  tmp[pos] = '\0';
  return pos;
}

static void s21_trim_trailing_zeros_mantissa(char *tmp) {
  // 1) Ищем экспоненту и отделяем её (если есть)
  int epos = -1;
  int len = 0;
  while (tmp[len] != '\0') {
    if (tmp[len] == 'e' || tmp[len] == 'E') {
      epos = len;
      break;
    }
    len++;
  }

  char expbuf[64];  // экспонента короткая: e±NNN
  int exp_len = 0;

  if (epos != -1) {
    // копируем экспоненту в expbuf
    int k = epos;
    while (tmp[k] != '\0' && exp_len < (int)sizeof(expbuf) - 1) {
      expbuf[exp_len++] = tmp[k++];
    }
    expbuf[exp_len] = '\0';

    // “обрываем” мантиссу перед экспонентой
    tmp[epos] = '\0';
    len = epos;  // длина мантиссы
  } else {
    expbuf[0] = '\0';
  }

  // 2) Тримим хвостовые нули в мантиссе
  int i = len - 1;
  while (i >= 0 && tmp[i] == '0') {
    tmp[i] = '\0';
    i--;
  }

  // если после удаления нулей осталась точка — убираем точку
  if (i >= 0 && tmp[i] == '.') {
    tmp[i] = '\0';
  }

  // 3) Приписываем экспоненту обратно (если была)
  if (expbuf[0] != '\0') {
    int new_len = 0;
    while (tmp[new_len] != '\0') new_len++;

    int j = 0;
    while (expbuf[j] != '\0') {
      tmp[new_len++] = expbuf[j++];
    }
    tmp[new_len] = '\0';
  }
}

static int s21_format_g(char *tmp, const s21_fmt_t *fs, long double v, int prec,
                        int upper) {
  if (s21_is_nan_ld(v) || s21_is_inf_ld(v))
    return s21_format_nan_inf(tmp, fs, v);

  // precision в %g/%G — это значащие цифры
  if (prec < 0) prec = 6;
  if (prec == 0) prec = 1;

  long double av = s21_fabsl(v);

  // exp10 = степень 10 в нормализованной записи (1.x * 10^exp10)
  int exp10 = 0;
  if (av != 0.0L) {
    long double norm = av;  // <-- важно: считаем степень на копии
    while (norm >= 10.0L) {
      norm /= 10.0L;
      exp10++;
    }
    while (norm < 1.0L) {
      norm *= 10.0L;
      exp10--;
    }
  }

  // правило выбора e/f для %g
  int use_e = (exp10 < -4 || exp10 >= prec);

  int len = 0;
  if (use_e) {
    // e: значащие цифры = 1 + (prec-1) после точки
    len = s21_format_e(tmp, fs, v, prec - 1, upper);
  } else {
    // f: кол-во цифр после точки так, чтобы всего было prec значащих
    int fprec = prec - (exp10 + 1);
    if (fprec < 0) fprec = 0;
    len = s21_format_f(tmp, fs, v, fprec);
  }

  // без '#' убираем хвостовые нули и возможную точку
  if (!fs->flag_hash) {
    s21_trim_trailing_zeros_mantissa(tmp);
    len = 0;
    while (tmp[len] != '\0') len++;
  }

  if (upper) s21_upper_str(tmp);
  return len;
}

// -------------------- vsprintf ядро --------------------

static int s21_vsprintf(char *str, const char *format, va_list ap_src) {
  va_list ap;
  va_copy(ap, ap_src);

  char *out = str;
  int written = 0;
  const char *f = format;

  while (*f != '\0') {
    if (*f != '%') {
      *out++ = *f++;
      written++;
      continue;
    }

    f++;
    if (*f == '\0') break;

    if (*f == '%') {
      *out++ = '%';
      written++;
      f++;
      continue;
    }

    s21_fmt_t fs;
    s21_fmt_init(&fs);
    s21_parse_flags(&f, &fs);
    s21_parse_width(&f, &fs, &ap);
    s21_parse_precision(&f, &fs, &ap);
    s21_parse_length(&f, &fs);

    fs.spec = *f;
    if (fs.spec == '\0') break;
    f++;

    if (fs.spec == 'n') {
      if (fs.length == 'h') {
        short *p = va_arg(ap, short *);
        if (p) *p = (short)written;
      } else if (fs.length == 'l') {
        long *p = va_arg(ap, long *);
        if (p) *p = (long)written;
      } else {
        int *p = va_arg(ap, int *);
        if (p) *p = (int)written;
      }
      continue;
    }

    char tmp[1024];
    int tmp_len = 0;

    if (fs.spec == 'c') {
      int ch = va_arg(ap, int); //типы, меньшие чем int увеличиваются до int
      tmp[0] = (char)ch; //...поэтому преобразовываем к char
      tmp[1] = '\0';
      tmp_len = 1;
      s21_emit_with_width(&out, &written, &fs, tmp, tmp_len, 0);
      continue;
    }

    if (fs.spec == 's') {
      const char *sp = va_arg(ap, const char *);
      if (sp == S21_NULL) sp = "(null)";
      //при отрицательном lim нет ограниченичений на точность, выведем всё
      int lim = (fs.precision >= 0) ? fs.precision : -1;
      int slen = s21_cstrlen_limit(sp, lim);

      int width = (fs.width < 0) ? 0 : fs.width;
      int pad = (width > slen) ? (width - slen) : 0;
      //при выравнивании справа выводим сначала заполнители - пробелы
      if (!fs.flag_minus) s21_out_repeat(&out, &written, ' ', pad);
      s21_out_mem(&out, &written, sp, slen);
      //при выравнивании слева выводим заполнители(пробелы) после строки
      if (fs.flag_minus) s21_out_repeat(&out, &written, ' ', pad);
      continue;
    }

    if (fs.spec == 'd' || fs.spec == 'i') {
      long long v;
      if (fs.length == 'h')
        v = (short)va_arg(ap, int);
      else if (fs.length == 'l')
        v = va_arg(ap, long);
      else
        v = va_arg(ap, int);

      tmp_len = s21_format_int(tmp, &fs, v, 0ULL, 1, 10, 0, 0);
      s21_emit_with_width(&out, &written, &fs, tmp, tmp_len, 0);
      continue;
    }

    if (fs.spec == 'u' || fs.spec == 'o' || fs.spec == 'x' || fs.spec == 'X') {
      unsigned long long v;
      if (fs.length == 'h')
        v = (unsigned short)va_arg(ap, unsigned int);
      else if (fs.length == 'l')
        v = va_arg(ap, unsigned long);
      else
        v = va_arg(ap, unsigned int);

      int base = 10;
      if (fs.spec == 'o') base = 8;
      if (fs.spec == 'x' || fs.spec == 'X') base = 16;
      int upper = (fs.spec == 'X');

      tmp_len = s21_format_int(tmp, &fs, 0LL, v, 0, base, upper, 0);
      s21_emit_with_width(&out, &written, &fs, tmp, tmp_len, 0);
      continue;
    }

    //указатель распечатывается как целое число в 16-чной системе счисления
    if (fs.spec == 'p') {
      void *pv = va_arg(ap, void *);
      unsigned long long v = (unsigned long long)(uintptr_t)pv;

      s21_fmt_t fs2 = fs;
      fs2.flag_hash = 0; //запрещаем указателю распечатываться как-то иначе, чем в 16-чной системе счисления
      fs2.spec = 'x';

      tmp_len = s21_format_int(tmp, &fs2, 0LL, v, 0, 16, 0, 1);
      s21_emit_with_width(&out, &written, &fs, tmp, tmp_len, 0);
      continue;
    }

    if (fs.spec == 'f' || fs.spec == 'e' || fs.spec == 'E' || fs.spec == 'g' ||
        fs.spec == 'G') {
      int prec = fs.precision;
      long double v =
          (fs.length == 'L') ? va_arg(ap, long double) : va_arg(ap, double);

      if (fs.spec == 'f')
        tmp_len = s21_format_f(tmp, &fs, v, prec);
      else if (fs.spec == 'e')
        tmp_len = s21_format_e(tmp, &fs, v, prec, 0);
      else if (fs.spec == 'E')
        tmp_len = s21_format_e(tmp, &fs, v, prec, 1);
      else if (fs.spec == 'g')
        tmp_len = s21_format_g(tmp, &fs, v, prec, 0);
      else
        tmp_len = s21_format_g(tmp, &fs, v, prec, 1);

      s21_emit_with_width(&out, &written, &fs, tmp, tmp_len, 1);
      continue;
    }

    // неизвестный spec — игнор
  }

  *out = '\0';
  va_end(ap);
  return written;
}

// -------------------- s21_sprintf --------------------

int s21_sprintf(char *str, const char *format, ...) {
  va_list ap; //список аргументов
  va_start(ap, format); //сохраняем начальное состояние списка аргументов
  int r = s21_vsprintf(str, format, ap);
  va_end(ap);
  return r;
}