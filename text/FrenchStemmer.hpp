#ifndef PAQ8PX_FRENCHSTEMMER_HPP
#define PAQ8PX_FRENCHSTEMMER_HPP

#include <cctype>
#include "Language.hpp"
#include "French.hpp"
#include "Word.hpp"
#include "Stemmer.hpp"

/**
 * French suffix stemmer, based on the Porter stemmer.
 */
class FrenchStemmer : public Stemmer {
private:
    static constexpr int NUM_VOWELS = 17;
    static constexpr char vowels[NUM_VOWELS] = {'a', 'e', 'i', 'o', 'u', 'y', '\xE2', '\xE0', '\xEB', '\xE9', '\xEA', '\xE8', '\xEF',
                                                '\xEE', '\xF4', '\xFB', '\xF9'};
    static constexpr int NUM_COMMON_WORDS = 10;
    const char *commonWords[NUM_COMMON_WORDS] = {"de", "la", "le", "et", "en", "un", "une", "du", "que", "pas"};
    static constexpr int NUM_EXCEPTIONS = 3;
    const char *(exceptions[NUM_EXCEPTIONS])[2] = {{"monument", "monument"},
                                                   {"yeux",     "oeil"},
                                                   {"travaux",  "travail"},};
    static constexpr uint32_t typesExceptions[NUM_EXCEPTIONS] = {French::Noun, French::Noun | French::Plural,
                                                                 French::Noun | French::Plural};
    static constexpr int NUM_SUFFIXES_STEP1 = 39;
    const char *suffixesStep1[NUM_SUFFIXES_STEP1] = {"ance", "iqUe", "isme", "able", "iste", "eux", "ances", "iqUes", "ismes", "ables",
                                                     "istes", //11
                                                     "atrice", "ateur", "ation", "atrices", "ateurs", "ations", //6
                                                     "logie", "logies", //2
                                                     "usion", "ution", "usions", "utions", //4
                                                     "ence", "ences", //2
                                                     "issement", "issements", //2
                                                     "ement", "ements", //2
                                                     "it\xE9", "it\xE9s", //2
                                                     "if", "ive", "ifs", "ives", //4
                                                     "euse", "euses", //2
                                                     "ment", "ments" //2
    };
    static constexpr int NUM_SUFFIXES_STEP2a = 35;
    const char *suffixesStep2A[NUM_SUFFIXES_STEP2a] = {"issaIent", "issantes", "iraIent", "issante", "issants", "issions", "irions",
                                                       "issais", "issait", "issant", "issent", "issiez", "issons", "irais", "irait",
                                                       "irent", "iriez", "irons", "iront", "isses", "issez", "\xEEmes", "\xEEtes", "irai",
                                                       "iras", "irez", "isse", "ies", "ira", "\xEEt", "ie", "ir", "is", "it", "i"};
    static constexpr int NUM_SUFFIXES_STEP2b = 38;
    const char *suffixesStep2B[NUM_SUFFIXES_STEP2b] = {"eraIent", "assions", "erions", "assent", "assiez", "\xE8rent", "erais", "erait",
                                                       "eriez", "erons", "eront", "aIent", "antes", "asses", "ions", "erai", "eras", "erez",
                                                       "\xE2mes", "\xE2tes", "ante", "ants", "asse", "\xE9"
                                                                                                     "es", "era", "iez", "ais", "ait",
                                                       "ant", "\xE9"
                                                              "e", "\xE9s", "er", "ez", "\xE2t", "ai", "as", "\xE9", "a"};
    static constexpr int NUM_SET_STEP4 = 6;
    static constexpr char setStep4[NUM_SET_STEP4] = {'a', 'i', 'o', 'u', '\xE8', 's'};
    static constexpr int NUM_SUFFIXES_STEP4 = 7;
    const char *suffixesStep4[NUM_SUFFIXES_STEP4] = {"i\xE8re", "I\xE8re", "ion", "ier", "Ier", "e", "\xEB"};
    static constexpr int NUM_SUFFIXES_STEP5 = 5;
    const char *suffixesStep5[NUM_SUFFIXES_STEP5] = {"enn", "onn", "ett", "ell", "eill"};

    inline bool isConsonant(char c);
    void convertUtf8(Word *w);
    void markVowelsAsConsonants(Word *w);
    uint32_t getRv(Word *w);
    bool step1(Word *w, uint32_t rv, uint32_t r1, uint32_t r2, bool *forceStep2A);
    bool step2A(Word *w, uint32_t rv);
    bool step2B(Word *w, uint32_t rv, uint32_t r2);
    static void step3(Word *w);
    bool step4(Word *w, uint32_t rv, uint32_t r2);
    bool step5(Word *w);
    bool step6(Word *w);
public:
    bool isVowel(char c) final;
    bool stem(Word *w) override;
};

#endif //PAQ8PX_FRENCHSTEMMER_HPP
