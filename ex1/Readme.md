# Relazione: Analisi delle Prestazioni degli Algoritmi di Ordinamento

## Introduzione

Questa relazione presenta l'analisi comparativa delle prestazioni di **Merge Sort** e **Quick Sort** su record con tre diversi campi di ordinamento:
- **Field 1**: Stringa (char)
- **Field 2**: Intero (int)  
- **Field 3**: Float (float)

## Metodologia

### Setup Sperimentale
- **Algoritmi**: Merge Sort vs Quick Sort
- **Dataset**: ~20 milioni di record
- **Campi testati**: 3 tipi di dati diversi

### Implementazioni
- **Merge Sort**: Algoritmo ricorsivo stabile, complessità O(n log n) garantita
- **Quick Sort**: Pivot ultimo elemento, complessità O(n log n) medio, O(n²) caso peggiore

## Risultati Sperimentali

### Tempi di Esecuzione Misurati

| Algoritmo | Field 1 (string) | Field 2 (int) | Field 3 (float) |
|-----------|-------------------|----------------|-----------------|
| **Merge Sort** | 18.5s | 16.9s | 16.0s |
| **Quick Sort** | 347s (5:47) | 14.2s | 14.3s |
| **Rapporto QS/MS** | **18.8x** | **0.84x** | **0.89x** |

### Analisi Statistica

**Merge Sort**:
- Range: 16.0s - 18.5s
- Variazione: 15%
- Comportamento: Stabile e predicibile

**Quick Sort**:
- Range: 14.2s - 347s  
- Variazione: 2340%
- Comportamento: Instabile con degenerazione su Field 1

## Confronto con la Teoria

### Risultati Coerenti

1. **Merge Sort Stabile**: Prestazioni consistenti O(n log n) su tutti i campi
2. **Field 1 più Lento**: Confronti string (`strcmp`) più costosi dei numerici
3. **Field 2/3 Simili**: Confronti numerici hanno prestazioni comparabili
4. **Quick Sort Efficiente**: Su dati favorevoli (Field 2/3) supera Merge Sort

### Degenerazione Algoritmica Critica

**Quick Sort su Field 1: 347 secondi**
- **18.8x più lento** di Merge Sort sullo stesso campo
- **24x più lento** rispetto agli altri campi
- Chiara degenerazione da O(n log n) a O(n²)

## Spiegazione del Comportamento Anomalo

### Causa Principale: Caso Peggiore O(n²)
I dati di Field 1 sono probabilmente **ordinati o quasi-ordinati** lessicograficamente. Con pivot fisso come ultimo elemento, Quick Sort degenera nel caso peggiore.

## Dipendenza dal Campo di Ordinamento

### Field 1 (String)
- **Merge Sort**: +15% rispetto ai campi numerici
- **Quick Sort**: +2340% (degenerazione completa)
- **Causa**: Costo `strcmp()` amplificato dalla degenerazione algoritmica

### Field 2 (Integer) e Field 3 (Float)
- **Prestazioni simili** per entrambi gli algoritmi
- **Quick Sort leggermente più veloce** di Merge Sort
- **Comportamento normale** O(n log n)

## Conclusioni

### Conferma della Teoria
I risultati confermano perfettamente la teoria algoritmica:
- **Merge Sort**: Garantisce O(n log n) indipendentemente dai dati
- **Quick Sort**: Prestazioni eccellenti su dati favorevoli, ma vulnerabile su dati sfavorevoli.
- **Importanza del tipo di confronto**: Le operazioni di confronto influenzano significativamente le prestazioni
