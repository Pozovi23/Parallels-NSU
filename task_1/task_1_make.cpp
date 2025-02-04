#include <iostream>
#include <cmath>

// #define TYPENAME float
#define TYPENAME double
#define AMOUNT_OF_ELEMENTS 10000000

int main() {
  TYPENAME *sinusMas = new TYPENAME[AMOUNT_OF_ELEMENTS];
  TYPENAME summ = 0;

  for (int i = 0; i < AMOUNT_OF_ELEMENTS; i++) {
    sinusMas[i] = std::sin((2 * M_PI * i) / AMOUNT_OF_ELEMENTS);
    summ += sinusMas[i];
  }

  std::cout << summ << std::endl;
  delete[] sinusMas;
  return 0;
}
