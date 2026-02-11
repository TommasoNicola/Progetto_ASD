#ifndef EDIT_DISTANCE_H
#define EDIT_DISTANCE_H

/**
 * Calculates the edit distance between two strings using a recursive approach.
 * Only deletion and insertion operations are allowed.
 * 
 * @param s1 The target string (string we want to obtain)
 * @param s2 The source string (string we start from)
 * @return The minimum edit distance between s1 and s2
 */
int edit_distance(const char *s1, const char *s2);

/**
 * Calculates the edit distance between two strings using dynamic programming.
 * Only deletion and insertion operations are allowed.
 * This version uses memoization for efficiency.
 * 
 * @param s1 The target string (string we want to obtain)
 * @param s2 The source string (string we start from)
 * @return The minimum edit distance between s1 and s2
 */
int edit_distance_dyn(const char *s1, const char *s2);

#endif
