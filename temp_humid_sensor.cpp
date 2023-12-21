#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include <iostream>

void print_word(std::string word) {
    puts(word.c_str());
}

int main()
{
    stdio_init_all();


    print_word("Peace");

    return 0;
}
