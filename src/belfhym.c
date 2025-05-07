/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Belphym.
 *
 * Belphym is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include <unistd.h>
#include <stdio.h>

void belfhym_greet(void) {
    printf("Welcome to Belfhym — the palm-sized flying machine!\n");
}

int main(void) {
    belfhym_greet();
    return 0;
}
