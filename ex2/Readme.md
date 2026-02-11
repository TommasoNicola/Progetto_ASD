# Edit Distance e Spell Checker

## Descrizione
Implementazione di un algoritmo di edit distance (solo inserimenti e cancellazioni) con applicazione a uno spell checker che suggerisce correzioni per parole non trovate in un dizionario.

## Struttura del Progetto

### File Principali
- **`edit_distance.h`** - Header con le dichiarazioni delle funzioni
- **`edit_distance.c`** - Implementazioni degli algoritmi di edit distance
- **`main_ex2.c`** - Applicazione spell checker
- **`test_ex2.c`** - Suite di test con Unity framework

## Scelte Implementative

### Algoritmi Edit Distance

#### 1. Versione Ricorsiva (`edit_distance`)
- **Approccio**: Ricorsione pura senza ottimizzazioni
- **Complessità**: O(3^(m+n)) - esponenziale
- **Uso**: Solo per test e confronti su stringhe brevi

#### 2. Versione Dinamica (`edit_distance_dyn`)
- **Approccio**: Memoizzazione top-down
- **Complessità**: O(m×n) - polinomiale
- **Struttura**: Helper function + tabella di cache
- **Gestione Memoria**: Allocazione/deallocazione automatica

### Spell Checker
#### Scelte Strutturali

**Normalizzazione Parole**:
- Rimozione punteggiatura
- Conversione in lowercase
- Mantiene originale per visualizzazione

**Gestione Dizionario**:
- Caricamento completo in memoria

**Algoritmo**:
1. Calcolo distanza minima globale
2. Raccolta parole alla distanza minima
3. Ordinamento alfabetico
4. Limite a MAX_SUGGESTIONS

#### Costanti di Configurazione
```c
#define MAX_WORD_LENGTH 100        // Lunghezza massima parola
#define MAX_DICTIONARY_SIZE 661562 // Dimensione massima dizionario
#define MAX_SUGGESTIONS 5          // Numero massimo suggerimenti
```

## Gestione Memoria

### Allocazione Dinamica
- **Dizionario**: `malloc` per array di stringhe
- **Memoizzazione**: Matrice 2D allocata dinamicamente
- **Cleanup**: `free` sistematico per evitare memory leak
