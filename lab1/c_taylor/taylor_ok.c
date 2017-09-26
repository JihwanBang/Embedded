//   Program Taylor0.c
// Compute sine() using the Taylor series with one to five terms.
//    Programmed by Byung Kook Kim

#include <stdio.h>
#include <math.h>

// Define PI
#define PI atan(1.)*4.
#define TERMS 5

int factorial(int m)
{
  int facto = 1;
  int i;
  for (i=2; i<=m; ++i)
    facto = facto*i;

  return facto;
}

int main()
{
  int indeg, facto;
  int pwr;
  int n, i, j;// Loop index
  double s0;
  double s[TERMS+1], e[TERMS+1];
  double dtr = PI/180.;
  double inrad, angpwr;
  
  // 1. Get input angle
  printf("Enter angle in degrees: ");
  scanf("%d", &indeg);
  
  // 2. Compute sine with Math library
  inrad = indeg*dtr;
  s0 = sin(inrad);
  printf("sin(%d)= %g using Math library.\n", indeg, s0);

  // 3. Compute sine using the Taylor series up to n terms
  for (n=1; n<=TERMS; ++n) {
    s[n] =0.0;
    // Loop to sum term i
    for (i=1; i<=n; ++i) {
      // Compute pwr
      pwr = 2*i - 1;
      // Compute angle^pwr
      angpwr = inrad;
      for (j=2; j<=i; ++j) angpwr *= inrad*inrad;//indeg*indeg;
      // Compute factorial pwr!
      facto = factorial(pwr);
      // Compute sine by Taylor series
      if (i%2 ==1)
	s[n] += angpwr/facto;
      else
	s[n] -= angpwr/facto;
    }
  }

  // 4. Compute error
  for (n=1; n<=TERMS; ++n)
    e[n] = s[n] - s0;

  // 5. Print result
  printf("sin(%d) using Taylor series:\n", indeg);
  for (i=1; i<=TERMS; ++i) {
    printf("%d terms: sin(%d)= %g, error= %g\n", i, indeg, s[i], e[i]);
  }

  return 0;
}

