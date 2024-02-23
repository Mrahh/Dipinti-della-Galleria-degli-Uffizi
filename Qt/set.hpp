/**
  @file set.hpp

  @brief File header della classe set templata

  File di dichiarazioni/definizioni della classe set templata
*/

#ifndef SET_HPP
#define SET_HPP

#include <algorithm> // per std::swap
#include <ostream>   // per std::ostream
#include <cassert>   // per assert
#include <fstream>   // per std::ofstream


/**
    @brief classe set 

    La classe implementa un generico set di oggetti T.
    _size rappresenta la dimensione del set
    _count rappresenta il numero di elementi presenti nel set
    _array è un puntatore all'array dinamico che contiene gli elementi del set
    _eql è un funtore che indica l'uguaglianza tra due oggetti di tipo T

*/
template <typename T, typename Equal> 
class set {

public:
    typedef unsigned int size_type; 
    
    typedef const T* const_iterator;
    
    const_iterator begin() const {
        return _array;
    }
    
    const_iterator end() const {
        return _array + _count;
    }

private:
    T* _array;
    size_type _size;
    size_type _count;
    Equal _eql;


    /** 
        @brief Funzione che ridimensiona il set al numero di elementi passato come parametro.

        Metodo di supporto alle funzioni add e remove, serve a ridimensionare il 
        set per agevolare l'aggiunta e la rimozione di elementi.
        Prende in input la nuova dimensione del set e ridimensiona il set a quella dimensione
        copiando gli elementi del vecchio set nel nuovo set.

        @param new_size nuova dimensione del set

        @post _size == new_size
        @post tmp[i] = _array[i]
    */
    void resize(size_type new_size) {
        // creo nuovo set con size elementi
        set tmp(new_size);
        
        // copio gli elementi
        for (size_type i = 0; i < _count; ++i)
            tmp.add(_array[i]);

        // sostituisco this con tmp
        swap(tmp);
    }
    
public:
    /**
        @brief funzione che restituisce il numero di elementi del set
        
        @return numero di elementi del set
    */
    size_type getNumElements() const {
        return _count;
    }

    int capacity(){
        return _size;
    }


    /**
       @brief Costruttore di default (metodo fondamentale)
       Construttore di default che inizializza il
       set ad un array dinamico vuoto.
       
       @post _array == nullptr
       @post _size == 0
       @post _count == 0
    */
    set() : _array(nullptr), _size(0), _count(0) {}


    /** 
        @brief Costruttore secondario.
        Permette di creare un set data la grandezza 

        @param size dimensione data

        @throws std::bad_alloc possibile eccezione di allocazione 
    */
    explicit set(size_type size) : _array(nullptr), _size(0), _count(0) {
        
        _array = new T[size];
        _size = size;
    }


    /** 
        @brief Copy constructor (metodo fondamentale)

        @param other set da copiare

        @throw std::bad_alloc possibile eccezione di allocazione

        @post _size == other._size
        @post _count == other._count
        @post tmp[i] = other._array[i]
    */
    set(const set &other) : _array(nullptr), _size(0), _count(0) {
        try {
            _array = new T[other._size];
            
            for (size_type i = 0; i < other._size; ++i)
                _array[i] = other._array[i];
            
            _size = other._size;
            _count = other._count;
        }
        catch(...) {
            // Se c'e' un problema, il set viene svuotato 
            // e l'eccezione viene rilanciata
            empty(); 
            throw;
        }
    }


    /**
        @brief Operatore di assegnamento (metodo fondamentale)
        Utilizzo della tecnica del copy-and-swap per implementare
        l'operatore di assegnamento.

        @param other set da copiare
        @return reference alla set this

        @post _size == other._size
        @post _count == other._count
        @post tmp[i] = other._array[i]
        
    */
    set& operator=(const set &other) {
        if (&other != this) {
            set tmp(other);
            swap(tmp); 
        }

        return *this;
    }


    /**
        @brief Distruttore (metodo fondamentale)

        @post _array == nullptr
        @post _size == 0
        @post _count == 0
    */
    ~set() {
        empty(); 
    }


    /**
        @brief Funzione che svuota la set deallocando la memoria occupata

        @post _array == nullptr
        @post _size == 0
        @post _count == 0
    */
    void empty() {
        delete[] _array;
        _array = nullptr;
        _size = 0;
        _count = 0;
    }
    

    /** 
        @brief funzione che scambia il contenuto di due set

        @param other set con cui scambiare il contenuto

        @post _size == other._size
        @post _count == other._count
        @post _array[i] = other._array[i] 
        @post _eq == other._eq
    */
    void swap(set &other) {
        std::swap(_size, other._size);
        std::swap(_count, other._count);
        std::swap(_array, other._array); 
        std::swap(_eql, other._eql);
    }


    /** 
        @brief Costruttore che crea una set riempita con dati
        presi da una sequenza identificata da un iteratore di 
        inizio e uno di fine. Gli iteratori possono essere di
        qualunque tipo. 

        @param begin iteratore di inizio sequenza
        @param end iteratore di fine sequenza

        @throw std::bad_alloc possibile eccezione di allocazione
    */
    template <typename Q>
    set(Q begin, Q end) : _array(nullptr), _size(0), _count(0) {
        Q curr = begin;
        try {
            for(; curr!=end; ++curr)
                add(static_cast<T>(*curr));
        }
        catch(...) {
            // Se c'e' un problema, il set viene svuotato 
            // e l'eccezione viene rilanciata
            empty();
            throw;
        }
    }


    /**
        @brief Funzione che aggiunge un elemento al set

        Se l'elemento è già presente nel set, non viene aggiunto.
        Se il set è pieno, viene riallocato con il doppio della capacità 
        e viene incrementato il numero di elementi.

        @param value valore da aggiungere al set

        @post _array[_count] == value
        @post _count == _count + 1

    */
    bool add(const T &value) {
        if (contains(value))
            return false;
        
        // ridimensionato il set se necessario
        if (_size == 0) 
            resize(1);
        else if(_count == _size)
            resize(2 * _size);
        
        _array[_count] = value;
        ++_count;

        return true;
    }


    /**
        @brief Funzione che rimuove un elemento dal set

        Se l'elemento non è presente nel set, non viene rimosso.

        Se il numero di elementi è pari a metà della dimensione del set, 
        il set viene ridimesionato a tre quarti della sua dimensione.

        Invece di creare un nuovo set in cui copiare gli elementi,
        l'elemento da rimuovere viene sostituito con l'ultimo elemento
        del set e viene decrementato il numero di elementi. 
        Questo funziona perché non è rilevante l'ordine degli elementi nel set.

        @param value valore da rimuovere dal set

        @post _count == _count - 1
    */
    bool remove(const T &value) {
        if (!contains(value))
            return false;
        
        // se l'elemento è presente, lo sostituisco con l'ultimo, il contatore viene aggiornato 
        // e viene restituito true per non controllare il resto degli elementi
        for (size_type i = 0; i < _count; ++i) {
            if (_eql(value, _array[i])) {
                _array[i] = _array[_count-1];
                _count = _count - 1;
                
                // ridimensioniamo il set se necessario
                if (_count <= _size / 2) 
                    resize(_size * 3 / 4);

                return true;
            }
        }

        return false;
    }


    /** 
        @brief Operatore di accesso ai dati.
        Permette l'accesso di sola lettura al set in posizione index.
        
        @param index indice dell'elemento da leggere
        
        @return reference all'elemento in posizione index
    */
    const T& operator[](const size_type index) const{
        assert(index < _count);
        
        return _array[index];
    }


    /** 
        @brief Operatore di uguaglianza tra due set. 
        Se un set contiene un numero di elementi diverso dall'altro,
        i due set non sono uguali. 
        Altrimenti controllo che tutti gli elementi di un set siano contenuti nell'altro.

        @param other set da confrontare con this

        @return true se i due set contengono gli stessi elementi, false altrimenti
    */
    bool operator==(const set &other) const {
        if (_count != other._count) {
            return false;
        }
        for (size_type i = 0; i < _count; ++i) 
            if (!other.contains(_array[i])) 
                return false;
            
        
        return true;
    }


    /** 
        @brief Funzione che controlla se un elemento è presente nel set.
        Se non ci sono elementi nel set, restituisce false. 
        Altrimenti controllo se l'elemento è presente.

        @param value valore da cercare nel set

        @return true se il set contiene l'elemento value, false altrimenti
    */
    bool contains(const T &value) const { 
        // se non ci sono elementi, viene restituito false
     
        for (size_type i = 0; i < _count; ++i)
            if (_eql(value, _array[i]))
                return true;
        
        return false;
    }


    /** 
        @brief Funzione GLOBALE che implementa l'operatore di stream.
        Permette di stampare il set su uno stream di output.
        La funzione è globale per poter accedere ai membri privati della classe.


        @param os stream di output
        @param set set da stampare

        @return lo stream di output 
    */
    friend std::ostream& operator<<(std::ostream &os, const set &set) {
        os << set._count;
        
        for (size_type i = 0; i < set._count; ++i)
            os << " (" << set[i] << ")";

        return os;
    }
};

/** 
    @brief Funzione che restituisce gli elementi del set che soddisfano il predicato

    @param st set da filtrare
    @param predicate predicato da soddisfare

    @return set filtrato
*/
template<typename T, typename Equal, typename Predicate>
set<T, Equal> filter_out(const set<T, Equal> &st, const Predicate predicate) {
    typename set<T, Equal>::const_iterator i, ie;

    set<T, Equal> result;

    for(i = st.begin(), ie = st.end(); i != ie; ++i) 
        if (predicate(*i)) 
            result.add(*i);


    return result;
}


/**
    @brief Operatore che ritorna il set che contiene gli elementi comuni ad entrambi i set
    Viene creato un nuovo set che ha come grandezza la somma degli elementi dei due set e
    vengono aggiunti gli elementi dei due set


    @param set1 primo set
    @param set2 secondo set

    @return set che contiene gli elementi di entrambi i set
*/
template<typename T, typename Equal>
set<T, Equal> operator+(const set<T, Equal> set1, const set<T, Equal> set2) {
    typename set<T, Equal>::const_iterator i = set1.begin(), ie = set1.end();
    
    // creo set di dimensione somma elementi dei set e li aggiungo al set
    set<T, Equal> result(set1.getNumElements() + set2.getNumElements());

    for(; i != ie; ++i)
        result.add(*i);

    for(i = set2.begin(); i != set2.end(); ++i)
        result.add(*i);
    
    return result;
}


/**
    @brief Operatore che dati due set ritorna il set che contiene gli elementi comuni ai due set.
    Quindi se un elemento è presente in entrambi i set, viene aggiunto nel set result.

    @param set1 primo set
    @param set2 secondo set

    @return set che contiene gli elementi comuni ai due set
*/
template<typename T, typename Equal>
set<T, Equal> operator-(const set<T, Equal> set1, const set<T, Equal> set2) {
    typename set<T, Equal>::const_iterator i = set1.begin(), ie = set1.end();
    set<T, Equal> result;

    for(; i != ie; ++i)
        if (set2.contains(*i))
            result.add(*i);

    return result;
}

/** 
    @brief Funzione che dato un set di stringhe e un nome di file, 
    salva il contenuto del set in un file di testo.

    Viene richiamato l'operatore << per stampare il set su file

    @param st set da salvare
    @param filename nome del file su cui salvare il set
    
    @throw possibile eccezione di apertura/lettura file

*/
template<typename Equal>
void save(const set<std::string, Equal> &st, const std::string filename) {
    std::ofstream myFile;
    try {
        myFile.open(filename);
        myFile << st;

        myFile.close();
    }
    catch(...) {
        if (myFile.is_open())
            myFile.close();
        throw;
    }
}


#endif