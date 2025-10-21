# 2025-07-24

-   artificial intelligence applicato al mondo della medicina
-   terapia che tratta un'aritmia cardiaca
-   sistema di mappaggio per il catetere nell'atrio
-   mesh della superficie intera dell'atrio
-   flatteria triale: onda che si propaga
-   ogni vertice della mesh ha come attributo una sequenza temporale di voltaggi
-   uno step: creare uno strumento per la visualizzazione 3d migliorata:
    -   magari in tempo reale
    -   magari con un'applicazione web (webgl)
-   formati dato:
    -   dalla macchina, contiene:
        -   geometria, connettività, normale dei vertici (e dei triangoli, calcolata)
        -   per vertice: misurazione unipolare, bipolare, LAT
    -   preprocessato

# 2025-09-24

-   core in c++, frontend javascript
-   traduzione da c++ a javascript in emscripten
-   primo task: da .mesh a .obj con c++: classe mesh con metodi import() e export() che prende come input i dati grezzi ed esporta in obj (o ply);

# 2025-10-15

_Osservazioni:_

-   _le mesh prodotte non sono two-manifold_

TODO:

-   pulizia del codice:
    -   rimuovere using namespace std
    -   (opt) creare metodo per aggiustare in manuale vertici non two manifold
    -   dividere la classe Mesh in una classe per la mesh e una funzione per l'import
    -   file con minuscola, classi con la maiuscola
    -   rimuovere vector3 e utilizzare glm::vec3
    -   rimuovere i getter e mettere gli attributi pubblici
-   ripulire a mano il vertice non two manifold (rinominare il file con \*fix\*)
-   aggiungere a vertex gli attributi SCI, ICL, ACL, EML, ExtEML, SCAR e fare accettare al metodo un parametro per decidere quale delle 6 possibili versioni della mesh in ply far esportare (messo in "property \<type\> quality" nell'header)

# 2025-10-15

-   convertire il codice c++ in javascript con enscripten
-   provare ad importare il .mesh dalla pagina web (.mesh -> buffer -> threejs)
-   fare il makefile (successivamente inserire il compilatore di emscripten: emcc)
