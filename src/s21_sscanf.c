#include <stdarg.h>  // va_list, va_start, va_arg, va_end

#include "s21_string.h"

/*
  s21_sscanf — моя реализация sscanf под проект.

  Идея простая:
  - есть указатель по формату (f) и указатель по входу (s)
  - читаю format слева направо:
      1) пробелы в формате -> пропускаю пробелы во входе
      2) обычный символ -> должен совпасть во входе
      3) '%' -> начинается "конверсия" (спецификатор): %d, %s, %x и т.д.
  - значения записываю в переменные пользователя через указатели из "..."
    (va_list / va_arg).

  Важно:
  - <string.h> не трогаю
  - память здесь не выделяю (всё пишет в буферы пользователя)
*/

// -------------------- мини-утилиты без <ctype.h> --------------------

static int s21_is_space_char(char c) {
    // Все "пробельные" символы (аналог isspace).
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' ||
            c == '\f');
}

static int s21_is_digit_char(char c) { return (c >= '0' && c <= '9'); }

static int s21_hex_val_char(char c) {
    // Превращаю символ hex-цифры в число 0..15.
    // Если символ не hex-цифра -> -1.
    if (c >= '0' && c <= '9') return (int)(c - '0');
    if (c >= 'a' && c <= 'f') return 10 + (int)(c - 'a');
    if (c >= 'A' && c <= 'F') return 10 + (int)(c - 'A');
    return -1;
}

static void s21_skip_spaces_input(const char **ps,
                                  __size_internal *read_chars) {
    /*
      Пропускаю пробелы во входе.
      ps — адрес указателя s (то есть я меняю сам s).
      read_chars увеличиваю на каждый реально пропущенный символ.
    */
    while (**ps != '\0' && s21_is_space_char(**ps)) {
        (*ps)++;
        (*read_chars)++;
    }
}

// -------------------- width в формате --------------------

static int s21_parse_width(const char **pf) {
    /*
      width — это цифры после % (например %10s).
      Если цифр нет, возвращаю -1 (значит "без ограничения").
    */
    int w = 0;
    int has = 0;
    while (**pf != '\0' && s21_is_digit_char(**pf)) {
        has = 1;
        w = w * 10 + (int)(**pf - '0');
        (*pf)++;
    }
    return has ? w : -1;
}

static int s21_width_ok(int width) {
    // width == -1 => лимита нет, можно читать
    // width == 0  => лимит исчерпан, читать нельзя
    return (width != 0);
}

static void s21_width_dec(int *width) {
    // Уменьшаю width только если он был задан числом (то есть > 0).
    if (*width > 0) (*width)--;
}

static int s21_can_consume_n(int width, int n) {
    // Могу ли прочитать n символов с учётом width?
    // width=-1 => да.
    if (width < 0) return 1;
    return width >= n;
}

// -------------------- length (h/l/L) --------------------

typedef enum { S21_LEN_NONE = 0, S21_LEN_H, S21_LEN_L, S21_LEN_LCAP } s21_len_t;

static s21_len_t s21_parse_length(const char **pf) {
    /*
      Модификаторы длины:
        %hd -> short
        %ld -> long
        %lf -> double (для float-спека)
        %Lf -> long double
    */
    if (**pf == 'h') {
        (*pf)++;
        return S21_LEN_H;
    }
    if (**pf == 'l') {
        (*pf)++;
        return S21_LEN_L;
    }
    if (**pf == 'L') {
        (*pf)++;
        return S21_LEN_LCAP;
    }
    return S21_LEN_NONE;
}

// -------------------- сборка unsigned числа в базе --------------------

static int s21_scan_unsigned_base(const char **ps, int base, int width,
                                  unsigned long long *out,
                                  __size_internal *read_chars) {
    /*
      Читаю последовательность цифр в базе base и собираю значение:

        val = val * base + digit

      Возвращаю 1, если прочитал хотя бы одну цифру.
      Возвращаю 0, если цифр не было.
    */
    const char *s = *ps;
    unsigned long long val = 0ULL;
    int got = 0;

    while (*s != '\0' && s21_width_ok(width)) {
        int d = -1;

        if (base == 16) {
            d = s21_hex_val_char(*s);
        } else {
            if (s21_is_digit_char(*s)) d = (int)(*s - '0');
        }

        if (d < 0 || d >= base) break;

        val = val * (unsigned long long)base + (unsigned long long)d;

        s++;  // "съедаю" символ входа
        (*read_chars)++;
        got = 1;

        s21_width_dec(&width);
    }

    if (!got) return 0;

    *out = val;
    *ps = s;
    return 1;
}

// -------------------- целые числа (%d/%i/%u/%o/%x/%p) --------------------

static int s21_scan_integer(const char **ps, int base, int auto_base,
                            int signed_mode, int width, long long *sout,
                            unsigned long long *uout,
                            __size_internal *read_chars) {
    /*
      Здесь делаю то, что нельзя сделать "голыми цифрами":
        - пропуск пробелов
        - знак +/-
        - 0x префикс
        - автобаза для %i
    */
    const char *s = *ps;

    s21_skip_spaces_input(&s, read_chars);
    if (*s == '\0') {
        *ps = s;
        return 0;
    }

    int w = width;

    int sign = 1;
    if (signed_mode && s21_width_ok(w) && (*s == '+' || *s == '-')) {
        if (*s == '-') sign = -1;
        s++;
        (*read_chars)++;
        s21_width_dec(&w);
    }

    int b = base;
    if (auto_base) {
        b = 10;
        if (s21_width_ok(w) && *s == '0') {
            if (s21_can_consume_n(w, 2) && (s[1] == 'x' || s[1] == 'X'))
                b = 16;
            else
                b = 8;
        }
    }

    if (b == 16) {
        if (s21_width_ok(w) && *s == '0' && s21_can_consume_n(w, 2) &&
            (s[1] == 'x' || s[1] == 'X')) {
            s += 2;
            (*read_chars) += 2;
            if (w > 0) w -= 2;
        }
    }

    unsigned long long uval = 0ULL;
    const char *before_digits = s;

    if (!s21_scan_unsigned_base(&s, b, w, &uval, read_chars)) {
        *ps = before_digits;
        return 0;
    }

    *ps = s;

    if (signed_mode) {
        long long sval = (sign == -1) ? -(long long)uval : (long long)uval;
        if (sout) *sout = sval;
    } else {
        if (uout) *uout = uval;
    }

    return 1;
}

// -------------------- float (%f/%e/%g) упрощённо --------------------

static long double s21_pow10_int(int exp) {
    long double p = 1.0L;
    if (exp >= 0) {
        for (int i = 0; i < exp; i++) p *= 10.0L;
    } else {
        for (int i = 0; i < -exp; i++) p /= 10.0L;
    }
    return p;
}

static int s21_scan_float(const char **ps, int width, long double *out,
                          __size_internal *read_chars) {
    const char *s = *ps;

    s21_skip_spaces_input(&s, read_chars);
    if (*s == '\0') {
        *ps = s;
        return 0;
    }

    int w = width;

    int sign = 1;
    if (s21_width_ok(w) && (*s == '+' || *s == '-')) {
        if (*s == '-') sign = -1;
        s++;
        (*read_chars)++;
        s21_width_dec(&w);
    }

    int got_any_digit = 0;

    unsigned long long int_part = 0ULL;
    while (*s != '\0' && s21_width_ok(w) && s21_is_digit_char(*s)) {
        got_any_digit = 1;
        int_part = int_part * 10ULL + (unsigned long long)(*s - '0');
        s++;
        (*read_chars)++;
        s21_width_dec(&w);
    }

    unsigned long long frac_part = 0ULL;
    int frac_digits = 0;

    if (*s != '\0' && s21_width_ok(w) && *s == '.') {
        s++;
        (*read_chars)++;
        s21_width_dec(&w);

        while (*s != '\0' && s21_width_ok(w) && s21_is_digit_char(*s)) {
            got_any_digit = 1;
            frac_part = frac_part * 10ULL + (unsigned long long)(*s - '0');
            frac_digits++;
            s++;
            (*read_chars)++;
            s21_width_dec(&w);
        }
    }

    if (!got_any_digit) {
        *ps = s;
        return 0;
    }

    long double val = (long double)int_part;
    if (frac_digits > 0)
        val += (long double)frac_part / s21_pow10_int(frac_digits);

    // e/E экспонента
    if (*s != '\0' && s21_width_ok(w) && (*s == 'e' || *s == 'E')) {
        const char *save = s;
        int save_w = w;
        __size_internal save_read = *read_chars;

        s++;
        (*read_chars)++;
        s21_width_dec(&w);

        int esign = 1;
        if (*s != '\0' && s21_width_ok(w) && (*s == '+' || *s == '-')) {
            if (*s == '-') esign = -1;
            s++;
            (*read_chars)++;
            s21_width_dec(&w);
        }

        int exp = 0;
        int exp_digits = 0;

        while (*s != '\0' && s21_width_ok(w) && s21_is_digit_char(*s)) {
            exp_digits = 1;
            exp = exp * 10 + (int)(*s - '0');
            s++;
            (*read_chars)++;
            s21_width_dec(&w);
        }

        if (exp_digits) {
            val *= s21_pow10_int(esign * exp);
        } else {
            // e было, а цифр после e не было -> откат
            s = save;
            w = save_w;
            *read_chars = save_read;
        }
    }

    *ps = s;
    *out = (sign == -1) ? -val : val;
    return 1;
}

// -------------------- сама s21_sscanf --------------------

int s21_sscanf(const char *str, const char *format, ...) {
    if (str == S21_NULL || format == S21_NULL) return 0;

    const char *s = str;
    const char *f = format;

    va_list ap;
    va_start(ap, format);

    int assigned = 0;
    __size_internal read_chars = 0;
    int any_conversion_attempted = 0;

    while (*f != '\0') {
        // пробел в формате => пропускаю пробелы во входе
        if (s21_is_space_char(*f)) {
            while (*f != '\0' && s21_is_space_char(*f)) f++;
            s21_skip_spaces_input(&s, &read_chars);
            continue;
        }

        // обычный символ должен совпасть
        if (*f != '%') {
            if (*s == *f) {
                s++;
                f++;
                read_chars++;
                continue;
            }
            break;
        }

        // конверсия
        f++;
        if (*f == '\0') break;

        // %%
        if (*f == '%') {
            any_conversion_attempted = 1;
            if (*s == '%') {
                s++;
                f++;
                read_chars++;
                continue;
            }
            break;
        }

        any_conversion_attempted = 1;

        int suppress = 0;
        if (*f == '*') {
            suppress = 1;
            f++;
        }

        int width = s21_parse_width(&f);
        s21_len_t len = s21_parse_length(&f);

        char spec = *f;
        if (spec == '\0') break;
        f++;

        if (spec != 'c' && spec != 'n') s21_skip_spaces_input(&s, &read_chars);

        // %n
        if (spec == 'n') {
            if (!suppress) {
                if (len == S21_LEN_H) {
                    short *p = va_arg(ap, short *);
                    *p = (short)read_chars;
                } else if (len == S21_LEN_L) {
                    long *p = va_arg(ap, long *);
                    *p = (long)read_chars;
                } else {
                    int *p = va_arg(ap, int *);
                    *p = (int)read_chars;
                }
                // assigned НЕ трогаю (как в libc).
            }
            continue;
        }

        // %c
        if (spec == 'c') {
            int count = (width > 0) ? width : 1;

            for (int i = 0; i < count; i++) {
                if (s[i] == '\0') {
                    va_end(ap);
                    return assigned;
                }
            }

            if (!suppress) {
                char *out = va_arg(ap, char *);
                for (int i = 0; i < count; i++) out[i] = s[i];
                assigned++;
            }

            for (int i = 0; i < count; i++) {
                s++;
                read_chars++;
            }
            continue;
        }

        // %s
        if (spec == 's') {
            if (*s == '\0') break;

            int w = width;
            int copied = 0;

            if (!suppress) {
                char *out = va_arg(ap, char *);
                while (*s != '\0' && !s21_is_space_char(*s) &&
                       (w < 0 || w > 0)) {
                    out[copied++] = *s;
                    s++;
                    read_chars++;
                    if (w > 0) w--;
                }
                if (copied == 0) break;
                out[copied] = '\0';
                assigned++;
            } else {
                while (*s != '\0' && !s21_is_space_char(*s) &&
                       (w < 0 || w > 0)) {
                    s++;
                    read_chars++;
                    if (w > 0) w--;
                    copied++;
                }
                if (copied == 0) break;
            }
            continue;
        }

        // целые
        if (spec == 'd' || spec == 'i' || spec == 'u' || spec == 'o' ||
            spec == 'x' || spec == 'X' || spec == 'p') {
            int base = 10, auto_base = 0, signed_mode = 0;

            if (spec == 'd') {
                base = 10;
                signed_mode = 1;
            }
            if (spec == 'i') {
                auto_base = 1;
                signed_mode = 1;
            }
            if (spec == 'u') {
                base = 10;
                signed_mode = 0;
            }
            if (spec == 'o') {
                base = 8;
                signed_mode = 0;
            }
            if (spec == 'x' || spec == 'X' || spec == 'p') {
                base = 16;
                signed_mode = 0;
            }

            const char *save_s = s;
            __size_internal save_read = read_chars;

            long long sval = 0;
            unsigned long long uval = 0;

            int ok = s21_scan_integer(&s, base, auto_base, signed_mode, width,
                                      &sval, &uval, &read_chars);
            if (!ok) {
                s = save_s;
                read_chars = save_read;
                break;
            }

            if (!suppress) {
                if (spec == 'p') {
                    void **pp = va_arg(ap, void **);
                    *pp = (void *)(__size_internal)uval;
                    assigned++;
                } else if (signed_mode) {
                    if (len == S21_LEN_H) {
                        short *p = va_arg(ap, short *);
                        *p = (short)sval;
                    } else if (len == S21_LEN_L) {
                        long *p = va_arg(ap, long *);
                        *p = (long)sval;
                    } else {
                        int *p = va_arg(ap, int *);
                        *p = (int)sval;
                    }
                    assigned++;
                } else {
                    if (len == S21_LEN_H) {
                        unsigned short *p = va_arg(ap, unsigned short *);
                        *p = (unsigned short)uval;
                    } else if (len == S21_LEN_L) {
                        unsigned long *p = va_arg(ap, unsigned long *);
                        *p = (unsigned long)uval;
                    } else {
                        unsigned int *p = va_arg(ap, unsigned int *);
                        *p = (unsigned int)uval;
                    }
                    assigned++;
                }
            }
            continue;
        }

        // float
        if (spec == 'f' || spec == 'e' || spec == 'E' || spec == 'g' ||
            spec == 'G') {
            const char *save_s = s;
            __size_internal save_read = read_chars;

            long double v = 0.0L;
            int ok = s21_scan_float(&s, width, &v, &read_chars);
            if (!ok) {
                s = save_s;
                read_chars = save_read;
                break;
            }

            if (!suppress) {
                if (len == S21_LEN_LCAP) {
                    long double *p = va_arg(ap, long double *);
                    *p = v;
                } else if (len == S21_LEN_L) {
                    double *p = va_arg(ap, double *);
                    *p = (double)v;
                } else {
                    float *p = va_arg(ap, float *);
                    *p = (float)v;
                }
                assigned++;
            }
            continue;
        }

        // неизвестный spec
        break;
    }

    va_end(ap);

    if (assigned == 0 && any_conversion_attempted && *s == '\0') return -1;
    return assigned;
}
