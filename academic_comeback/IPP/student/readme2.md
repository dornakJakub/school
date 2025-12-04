Implementační dokumentace k 2. úloze do IPP 2024/2025  
Jméno a příjmení: Jakub Dorňák  
Login: xdorna06


## Struktura projektu

Projekt implementuje třídu `MyObjectInstance` reprezentujíci třídu `Object` jazyka SOL25, z ní dědí veškeré vestavěné třídy. Projekt používá Utility třídu IOManager pro jednoduché předání I/O objektů.

## Zpracování XML

Celé XML se na začátku předá instanci třídy `ClassManager`, která z něj vypreparuje definice všech tříd a metod, ze kterých vytvoří instance `ClassTemplate` a `Method` které si nadále třída `ClassManager` drží a je možno je od ní poptávat. Dále se Vytvoří instance `Main` a spustí se metoda `run`. Interpret má pro každou metodu instanci třídy `Block` podle které, při vyhodnocování daného bloku vytváří instanci `MyBlockInstance`. Ta postupně vyhodnocuje každý *assign* element v daném bloku. Pro vyhodnocení *expr* elementů se používá třída `Expr` Která dále vytváří instance sama sebe dokud se nezanoří na nejnižší element právě prováděné větve XML stromu. Každá instance `Expr` se pak vyhodnotí a vrátí výsledek typu `MyObjectInstance` své nadřazené instanci dokud neprojde stromem až k instanci `MyBlockInstance` která výsledek přiřazení uloži do své lokální paměti.

## Nedokonalosti implementace

Není naimplementováno správné vyhodnocení metod instancí uživatelem definovaných tříd.  
Není implementována vestavěná metoda třídy Block whileTrue: