#include "../lib/unity/unity.h"
#include "edit_distance.h"

// Called before each test
void setUp(void) {}

// Called after each test
void tearDown(void) {}

// Recursive implementation tests
void test_recursive_edit_distance_casa_cassa(void) {
    TEST_ASSERT_EQUAL_INT(1, edit_distance("casa", "cassa"));
}

void test_recursive_edit_distance_casa_cara(void) {
    TEST_ASSERT_EQUAL_INT(2, edit_distance("casa", "cara"));
}

void test_recursive_edit_distance_vinaio_vino(void) {
    TEST_ASSERT_EQUAL_INT(2, edit_distance("vinaio", "vino"));
}

void test_recursive_edit_distance_tassa_passato(void) {
    TEST_ASSERT_EQUAL_INT(4, edit_distance("tassa", "passato"));
}

void test_recursive_edit_distance_same_word(void) {
    TEST_ASSERT_EQUAL_INT(0, edit_distance("pioppo", "pioppo"));
}

void test_recursive_edit_distance_empty_strings(void) {
    TEST_ASSERT_EQUAL_INT(0, edit_distance("", ""));
    TEST_ASSERT_EQUAL_INT(4, edit_distance("casa", ""));
    TEST_ASSERT_EQUAL_INT(5, edit_distance("", "cassa"));
}

// Dynamic programming implementation tests
void test_dynamic_edit_distance_casa_cassa(void) {
    TEST_ASSERT_EQUAL_INT(1, edit_distance_dyn("casa", "cassa"));
}

void test_dynamic_edit_distance_casa_cara(void) {
    TEST_ASSERT_EQUAL_INT(2, edit_distance_dyn("casa", "cara"));
}

void test_dynamic_edit_distance_vinaio_vino(void) {
    TEST_ASSERT_EQUAL_INT(2, edit_distance_dyn("vinaio", "vino"));
}

void test_dynamic_edit_distance_tassa_passato(void) {
    TEST_ASSERT_EQUAL_INT(4, edit_distance_dyn("tassa", "passato"));
}

void test_dynamic_edit_distance_same_word(void) {
    TEST_ASSERT_EQUAL_INT(0, edit_distance_dyn("pioppo", "pioppo"));
}

void test_dynamic_edit_distance_empty_strings(void) {
    TEST_ASSERT_EQUAL_INT(0, edit_distance_dyn("", ""));
    TEST_ASSERT_EQUAL_INT(4, edit_distance_dyn("casa", ""));
    TEST_ASSERT_EQUAL_INT(5, edit_distance_dyn("", "cassa"));
}

// Cross-check consistency between recursive and dynamic implementations
void test_consistency_between_implementations(void) {
    TEST_ASSERT_EQUAL_INT(edit_distance("abc", "ab"), edit_distance_dyn("abc", "ab"));
    TEST_ASSERT_EQUAL_INT(edit_distance("algorithm", "altruistic"), edit_distance_dyn("algorithm", "altruistic"));
    TEST_ASSERT_EQUAL_INT(edit_distance("kitten", "sitting"), edit_distance_dyn("kitten", "sitting"));
}

int main(void) {
    UNITY_BEGIN();

    // Recursive tests
    RUN_TEST(test_recursive_edit_distance_casa_cassa);
    RUN_TEST(test_recursive_edit_distance_casa_cara);
    RUN_TEST(test_recursive_edit_distance_vinaio_vino);
    RUN_TEST(test_recursive_edit_distance_tassa_passato);
    RUN_TEST(test_recursive_edit_distance_same_word);
    RUN_TEST(test_recursive_edit_distance_empty_strings);

    // Dynamic programming tests
    RUN_TEST(test_dynamic_edit_distance_casa_cassa);
    RUN_TEST(test_dynamic_edit_distance_casa_cara);
    RUN_TEST(test_dynamic_edit_distance_vinaio_vino);
    RUN_TEST(test_dynamic_edit_distance_tassa_passato);
    RUN_TEST(test_dynamic_edit_distance_same_word);
    RUN_TEST(test_dynamic_edit_distance_empty_strings);

    // Consistency check
    RUN_TEST(test_consistency_between_implementations);

    return UNITY_END();
}
