Implementační dokumentace k 1. úloze do IPP 2023/2024
Jméno a příjmení: Jakub Dorňák
Login: xdorna06

## Parse.py

Skript nejprve provede kontrolu argumentu `--help`. Vzhledem k tomu že není naimplementováno žádné rozšíření skriptu, je k tomuto použita knihovna `sys`

Zdrojový kód musí obsahovat přesně jednu hlavičku, jinak skript končí chybou 23.

Po kontrole hlavičky je vytvořen kořen xml stromu. Pro generaci xml souboru je použita knihovna `xml.etree.ElementTree`. Tato knihovna je použita zejména kvůli její rozšířenosti a popularitě.

Následně započne cyklus který po řádcích načítá zdrojový kód z `stdin`. Z každého řádku je nejprve odstraněn případný komentář, poté je určeno o jakou instrukci jde. Dále proběhne kontrola počtu argumentů a kontrola typů `var, sym` a `label` Pomocí regulárních výrazů. Poté je vytvořen `subElement` kořene xml stromu s informacemi o instrukci.

Instrukce očekávající stejný formát argumentů jsou sjednoceny do seznamů. Instrukce ze vstupu jsou poté porovnávány vůči celému seznamu.

Dále dojde ke zpracování argumentů. Nejprve je specifikován datový typ argumentu pomocí regulárních výrazů. Pro každý argument je vytvořen `subElement` instrukce se kterou je volán.

Po celou dobu cyklu je udržován čítač instrukcí, který se inkrementuje s každým neprázdným řádkem

Cyklus končí přečtením a zpracováním poslední řádky vstupního souboru. Nakonec je do `stdout` vypsána xml hlavička a výsledný xml strom.

### Možné vylepšení skriptu:
Skript by se dal vylepšit použitím OOP, které by pomohlo eliminovat duplicitní kód. Dalším způsobem zlepšení kvality kódu by bylo implementací několika vhodných funkcí.