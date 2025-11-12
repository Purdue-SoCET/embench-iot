#pragma once

void format(const char *fmt, char *buf, ...);
void print(const char *fmt, ...);
void __attribute__((noinline)) print_string(const char *s);
