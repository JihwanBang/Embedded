//   Program Taylor0.c
// Compute sine() using the Taylor series with one to five terms.
//    Programmed by Byung Kook Kim

#include <stdio.h>
#include <math.h>

// Define PI
#define PI 3.14
#define TERMS5

int factorial(int m)
{
  int facto = 1;
  for (i=2; i<=m; ++i)
    facto = facto*i;

  return facto;
}

int main()
{
  int indeg;
  int pwr;
  int n, i;// Loop index

  // 1. Get input angle
  printf("Enter angle in degrees: ");
  scanf("%d", &indeg);

  // 2. Compute sine with Math library
  s0 = sin(indeg);
  printf("sin(%d)= %g using Math library.\n", indeg);

  // 3. Compute sine using the Taylor series up to n terms
  for (n=1; n<=TERM; ++n) {
    // Loop to sum term i
    for (i=1; j<=n; ++i) {
      // Compute pwr
      pwr = 2*i - 1;
      // Compute angle^pwr
      angpwr = indeg;
      for (j=2; j<=i; ++j) angpwr *= indeg*indeg;
      // Compute factorial pwr!
      facto = factorial(pwr);
      // Compute sine by Taylor series
      s[i] += angpwr/facto;
    }
  }

  // 4. Compute error
  for (n=1; n<=TERM; ++n)
    e[i] = s[i] - s0;

  // 5. Print result
  printf("sin(%d) using Taylor series:\n", indeg);
  for (i=1; i<=TERMS; ++i) {
    printf("%d terms: sin(%d)= %g, error= %g\n", i, indeg, s[i], e[i]);
  }

  return 0;
}

