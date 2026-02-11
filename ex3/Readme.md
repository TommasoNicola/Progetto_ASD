# Esercizio 3 – Tavole Hash (con concatenamento)

## Descrizione del progetto

Questo progetto implementa una libreria generica per una **tavola hash con concatenamento**, in grado di gestire coppie chiave-valore con tipi generici. La libreria supporta le principali operazioni di inserimento, ricerca, rimozione e accesso alle chiavi. Il progetto include anche:

- Un programma (`main_ex3`) per trovare la parola più frequente (di almeno una certa lunghezza) in un file di testo.
- Un insieme di **unit test automatizzati** (`test_ex3`) per validare il corretto funzionamento della struttura dati.
- Un file `Makefile` per automatizzare la compilazione.

## Specifiche implementative

Le funzioni implementate nella libreria sono:

```c
HashTable* hash_table_create(int (*f1)(const void*, const void*), unsigned long (*f2)(const void*));
void hash_table_put(HashTable*, const void*, const void*);
void* hash_table_get(const HashTable*, const void*);
int hash_table_contains_key(const HashTable*, const void*);
void hash_table_remove(HashTable*, const void*);
int hash_table_size(const HashTable*);
void** hash_table_keyset(const HashTable*);
void hash_table_free(HashTable*);
```

## Promt iniziale al LLM

"Scrivi in C una struttura dati hash table con concatenamento, supportando chiavi e valori generici. Deve supportare inserimento, ricerca, rimozione, dimensione e accesso all'insieme delle chiavi. Devono essere passate due funzioni: una di hash e una di confronto."

## Output iniziale prodotto dal LLM

Il sistema ha fornito una bozza iniziale di implementazione della struttura, completa di:

- Struttura `HashTable`.
- Nodo `HashNode`.
- Implementazione di funzioni come put, get, remove.
- Uso di funzioni generiche per hash e confronto.
- Alcune funzioni di utilità (es. keyset, size).

## Analisi critica dell'output

- **Correttezza**: il codice era strutturalmente corretto, ma mancavano alcune verifiche di sicurezza e gestione degli errori (es. controllo puntatori nulli).
- **Modularità**: il codice era ben separato in funzioni, ma abbiamo migliorato la documentazione delle interfacce.
- **Testabilità**: l'output non conteneva unit test, quindi sono stati aggiunti usando la libreria Unity.

## Modifiche del prompt

Per la gestione degli errori:

"Scrivi in C una struttura dati hash table con concatenamento, supportando chiavi e valori generici. Deve supportare inserimento, ricerca, rimozione, dimensione e accesso all'insieme delle chiavi. Devono essere passate due funzioni: una di hash e una di confronto. Assicurati di gestire correttamente i puntatori nulli e gli errori."

Il LLM ha risposto con un codice migliorato, includendo controlli per puntatori nulli e gestione degli errori, utilizzando stderrno per segnalare errori e exit() per terminare il programma in caso di errori critici.

Per generare i test:

"Genera unit test int C per una struttura dati hash table con concatenamento, che supporta chiavi e valori generici. I test devono coprire inserimento, ricerca, rimozione, dimensione e accesso all'insieme delle chiavi. Usa la libreria Unity per i test."

Il LLM ha prodotto una serie di test che coprono i casi principali, inclusi inserimenti, ricerche, rimozioni e verifiche di dimensione. I test sono stati organizzati in modo da essere facilmente eseguibili e leggibili con Unity.

Per il programma che legge un file di testo:

"Scrivi un main in C che legge un file di testo e usa una hash table per trovare la parola più frequente con lunghezza almeno pari a un valore dato."

Il LLM ha generato un file `main_ex3.c` che implementa la logica per leggere un file di testo, contare le parole e trovare quella più frequente con una lunghezza minima specificata. Il codice gestisce correttamente l'apertura del file, la lettura delle parole e l'uso della hash table per il conteggio.

## Considerazioni generali sull'uso di LLM

### Vantaggi:

- **Rapidità di prototipazione**: il LLM ha accelerato notevolmente la scrittura del codice iniziale, fornendo una base solida da cui partire.
- **Supporto nella scrittura dei test**: ha generato test unitari che hanno semplificato la verifica del funzionamento della struttura dati.
- **Assistenza nella gestione dei tipi generici in C**: che è complesso e richiede attenzione ai dettagli.

### Svantaggi:

- **Mancanza di contesto**: il LLM non ha sempre compreso appieno le specifiche del progetto, richiedendo iterazioni per ottenere il risultato desiderato. Alcune scelte progettuali sono state migliorate manualmente.
- **Gestione degli errori**: inizialmente ilcodice generato non gestiva correttamente gli errori, richiedendo ulteriori modifiche per garantire robustezza.

## Conclusioni

L'uso di LLM per la scrittura di codice C ha dimostrato di essere un valido supporto, soprattutto per compiti ripetitivi e per la generazione di test. Tuttavia, è necessario mantenere un controllo sul codice generato, specialmente in contesti complessi come la gestione di strutture dati generiche. La combinazione di automazione e revisione manuale ha portato a un risultato finale ben strutturato e funzionante, con una buona copertura di test e gestione degli errori.
