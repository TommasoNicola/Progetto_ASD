#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "edit_distance.h"

// Recursive function to calculate edit distance
int edit_distance(const char *s1, const char *s2) {
  if(*s1 == '\0') return strlen(s2);
  if(*s2 == '\0') return strlen(s1);

  int d_no_op = INT_MAX;
  if (*s1 == *s2) {
    d_no_op = edit_distance(s1 + 1, s2 + 1);
  }

  int d_canc = 1 + edit_distance(s1, s2 + 1);
  int d_ins = 1 + edit_distance(s1 + 1, s2);

  int min = d_no_op;
  if (d_canc < min) min = d_canc;
  if (d_ins < min) min = d_ins;

  return min;
}

// Helper function for dynamic programming version
int edit_distance_dyn_helper(const char *s1, const char *s2, int len1, int len2, int **memo) {
  if (len1 == 0) return len2; // If s1 is empty, return length of s2
  if (len2 == 0) return len1; // If s2 is empty, return length of s1

  if (memo[len1][len2] != -1) return memo[len1][len2]; // Return already computed value

  int d_no_op = INT_MAX;
  if (s1[len1 - 1] == s2[len2 - 1]) {
    d_no_op = edit_distance_dyn_helper(s1, s2, len1 - 1, len2 - 1, memo);
  }

  int d_canc = 1 + edit_distance_dyn_helper(s1, s2, len1, len2 - 1, memo);
  int d_ins = 1 + edit_distance_dyn_helper(s1, s2, len1 - 1, len2, memo);

  int min = d_no_op;
  if (d_canc < min) min = d_canc;
  if (d_ins < min) min = d_ins;
  memo[len1][len2] = min; // Store the computed value in the memoization table
                          
  return min;
}

// Dynamic programming version with memoization
int edit_distance_dyn(const char *s1, const char *s2) {
  int len1 = strlen(s1);
  int len2 = strlen(s2);
  
  // Create a memoization table
  int **memo = (int **)malloc((len1 + 1) * sizeof(int *));
  for (int i = 0; i <= len1; i++) {
    memo[i] = (int *)malloc((len2 + 1) * sizeof(int));
    for (int j = 0; j <= len2; j++) {
      memo[i][j] = -1; // Initialize with -1 to indicate uncomputed values
    }
  }

  // Helper function for memoized recursion
  int result = edit_distance_dyn_helper(s1, s2, len1, len2, memo);

  // Free the memoization table
  for (int i = 0; i <= len1; i++) {
    free(memo[i]);
  }
  free(memo);

  return result;
}
