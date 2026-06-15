-- ====================================================================
-- IDS - Projekt 4. cast | Tema: 21 Knihovna1 | xderned00, xdorna06
-- ====================================================================

-- cleanup
DROP MATERIALIZED VIEW mv_statistika_titulu;
DROP INDEX idx_vypujcka_prukaz;
DROP TABLE Titul_Autor CASCADE CONSTRAINTS;
DROP TABLE Vypujcka CASCADE CONSTRAINTS;
DROP TABLE Rezervace CASCADE CONSTRAINTS;
DROP TABLE Exemplar CASCADE CONSTRAINTS;
DROP TABLE Kniha CASCADE CONSTRAINTS;
DROP TABLE Casopis CASCADE CONSTRAINTS;
DROP TABLE Titul CASCADE CONSTRAINTS;
DROP TABLE Autor CASCADE CONSTRAINTS;
DROP TABLE Ctenar CASCADE CONSTRAINTS;
DROP TABLE Pracovnik CASCADE CONSTRAINTS;
DROP TABLE Uzivatel CASCADE CONSTRAINTS;
DROP SEQUENCE seq_vypujcka;

/* ================================================================================
 VYSVETLENIE PREVODU GENERALIZACIE / SPECIALIZACIE
================================================================================
 V scheme mame vztahy nadtyp-podtyp:
 1. Uzivatel -> Ctenar a Pracovnik
 2. Titul -> Kniha a Casopis

 Riesenie v relacnej databaze:
 Pre nadtyp aj kazdy podtyp sme vytvorili samostatnu tabulku. 
 Tabulka podtypu obsahuje len svoje specificke data. 
 Primarny kluc podtypu sluzi zaroven ako cudzi kluc (FOREIGN KEY), 
 ktory odkazuje na primarny kluc nadtypu. Takto su data prepojene.
================================================================================
*/

-- vytvoreni tabulek a omezenei
CREATE TABLE Uzivatel (
    ID_uzivatele INT PRIMARY KEY,
    Jmeno VARCHAR2(50) NOT NULL,
    Prijmeni VARCHAR2(50) NOT NULL,
    Email VARCHAR2(100) NOT NULL UNIQUE,
    Heslo VARCHAR2(255) NOT NULL
);

CREATE TABLE Ctenar (
    ID_prukazu INT PRIMARY KEY,
    Datum_registrace DATE NOT NULL,
    ID_uzivatele INT NOT NULL UNIQUE,
    CONSTRAINT fk_ctenar_uzivatel FOREIGN KEY (ID_uzivatele) REFERENCES Uzivatel(ID_uzivatele)
);

CREATE TABLE Pracovnik (
    ID_pracovnika INT PRIMARY KEY,
    Cislo_kancelare VARCHAR2(20),
    Pozice VARCHAR2(50),
    ID_uzivatele INT NOT NULL UNIQUE,
    CONSTRAINT fk_pracovnik_uzivatel FOREIGN KEY (ID_uzivatele) REFERENCES Uzivatel(ID_uzivatele)
);

CREATE TABLE Autor (
    ID_autora INT PRIMARY KEY,
    Jmeno VARCHAR2(50) NOT NULL,
    Prijmeni VARCHAR2(50) NOT NULL,
    Narodnost VARCHAR2(50)
);

CREATE TABLE Titul (
    ID_titulu INT PRIMARY KEY,
    Nazev VARCHAR2(255) NOT NULL,
    Rok_vydani INT,
    Vydavatel VARCHAR2(100)
);

CREATE TABLE Kniha (
    ID_titulu INT PRIMARY KEY,
    ISBN VARCHAR2(20) NOT NULL,
    Pocet_stran INT,
    CONSTRAINT fk_kniha_titul FOREIGN KEY (ID_titulu) REFERENCES Titul(ID_titulu) ON DELETE CASCADE,
    -- SPECIÁLNÍ OMEZENÍ: Kontrola formátu ISBN-13 (např. 978-80-200-0980-7)
    CONSTRAINT chk_kniha_isbn CHECK (REGEXP_LIKE(ISBN, '^(978|979)-[0-9]{1,5}-[0-9]{1,7}-[0-9]{1,7}-[0-9]$'))
);

CREATE TABLE Casopis (
    ID_titulu INT PRIMARY KEY,
    ISSN VARCHAR2(9) NOT NULL,
    Cislo_vydani INT,
    CONSTRAINT fk_casopis_titul FOREIGN KEY (ID_titulu) REFERENCES Titul(ID_titulu) ON DELETE CASCADE,
    -- SPECIÁLNÍ OMEZENÍ: Kontrola formátu ISSN (4 číslice, pomlčka, 3 číslice a číslice nebo X)
    CONSTRAINT chk_casopis_issn CHECK (REGEXP_LIKE(ISSN, '^[0-9]{4}-[0-9]{3}[0-9X]$'))
);

-- vazbova tabulka medzi titulom a autorom n:n
CREATE TABLE Titul_Autor (
    ID_titulu INT,
    ID_autora INT,
    PRIMARY KEY (ID_titulu, ID_autora),
    CONSTRAINT fk_ta_titul FOREIGN KEY (ID_titulu) REFERENCES Titul(ID_titulu) ON DELETE CASCADE,
    CONSTRAINT fk_ta_autor FOREIGN KEY (ID_autora) REFERENCES Autor(ID_autora) ON DELETE CASCADE
);

CREATE TABLE Exemplar (
    ID_exemplare INT PRIMARY KEY,
    Stav VARCHAR2(50) NOT NULL,
    ID_titulu INT NOT NULL,
    CONSTRAINT fk_exemplar_titul FOREIGN KEY (ID_titulu) REFERENCES Titul(ID_titulu) ON DELETE CASCADE
);

CREATE TABLE Rezervace (
    ID_rezervace INT PRIMARY KEY,
    Datum_vytvoreni DATE NOT NULL,
    Stav VARCHAR2(20) NOT NULL,
    ID_prukazu INT NOT NULL,
    ID_titulu INT NOT NULL,
    CONSTRAINT fk_rez_ctenar FOREIGN KEY (ID_prukazu) REFERENCES Ctenar(ID_prukazu),
    CONSTRAINT fk_rez_titul FOREIGN KEY (ID_titulu) REFERENCES Titul(ID_titulu)
);

CREATE TABLE Vypujcka (
    ID_vypujcky INT PRIMARY KEY,
    Datum_start DATE NOT NULL,
    Datum_konec DATE NOT NULL,
    Vraceno CHAR(1) DEFAULT 'N' CHECK (Vraceno IN ('A', 'N')),
    ID_exemplare INT NOT NULL,
    ID_prukazu INT NOT NULL,
    ID_pracovnika INT, 
    CONSTRAINT fk_vyp_exemplar FOREIGN KEY (ID_exemplare) REFERENCES Exemplar(ID_exemplare),
    CONSTRAINT fk_vyp_ctenar FOREIGN KEY (ID_prukazu) REFERENCES Ctenar(ID_prukazu),
    CONSTRAINT fk_vyp_pracovnik FOREIGN KEY (ID_pracovnika) REFERENCES Pracovnik(ID_pracovnika)
);


-- generovani hodnot
CREATE SEQUENCE seq_vypujcka START WITH 1 INCREMENT BY 1;

CREATE OR REPLACE TRIGGER trg_vypujcka_pk
BEFORE INSERT ON Vypujcka
FOR EACH ROW
BEGIN
	-- ak uzivatel nevlozi ID, (null), vygeneruje sa zo sekvencie automaticky
    IF :NEW.ID_vypujcky IS NULL THEN
        :NEW.ID_vypujcky := seq_vypujcka.NEXTVAL;
    END IF;
END;
/

-- vlozeni dat

INSERT INTO Uzivatel (ID_uzivatele, Jmeno, Prijmeni, Email, Heslo) VALUES (1, 'Jan', 'Novák', 'jan.novak@email.cz', 'hash123');
INSERT INTO Uzivatel (ID_uzivatele, Jmeno, Prijmeni, Email, Heslo) VALUES (2, 'Eva', 'Svobodová', 'eva.svoboda@knihovna.cz', 'hash456');

INSERT INTO Ctenar (ID_prukazu, Datum_registrace, ID_uzivatele) VALUES (1001, TO_DATE('2023-01-15', 'YYYY-MM-DD'), 1);
INSERT INTO Pracovnik (ID_pracovnika, Cislo_kancelare, Pozice, ID_uzivatele) VALUES (2001, 'Kanc-A1', 'Knihovník', 2);

INSERT INTO Autor (ID_autora, Jmeno, Prijmeni, Narodnost) VALUES (1, 'Karel', 'Čapek', 'Česká');
INSERT INTO Autor (ID_autora, Jmeno, Prijmeni, Narodnost) VALUES (2, 'George', 'Orwell', 'Britská');

INSERT INTO Titul (ID_titulu, Nazev, Rok_vydani, Vydavatel) VALUES (1, 'R.U.R.', 1920, 'Borový');
INSERT INTO Titul (ID_titulu, Nazev, Rok_vydani, Vydavatel) VALUES (2, 'National Geographic', 2023, 'NG Partners');

INSERT INTO Kniha (ID_titulu, ISBN, Pocet_stran) VALUES (1, '978-80-7390-035-7', 120); 
INSERT INTO Casopis (ID_titulu, ISSN, Cislo_vydani) VALUES (2, '0027-9358', 5);

INSERT INTO Titul_Autor (ID_titulu, ID_autora) VALUES (1, 1);

INSERT INTO Exemplar (ID_exemplare, Stav, ID_titulu) VALUES (1, 'Dobrý', 1);
INSERT INTO Exemplar (ID_exemplare, Stav, ID_titulu) VALUES (2, 'Poškozený', 1);
INSERT INTO Exemplar (ID_exemplare, Stav, ID_titulu) VALUES (3, 'Nový', 2);

INSERT INTO Rezervace (ID_rezervace, Datum_vytvoreni, Stav, ID_prukazu, ID_titulu) 
VALUES (1, TO_DATE('2023-10-01', 'YYYY-MM-DD'), 'Aktivní', 1001, 1);

INSERT INTO Vypujcka (ID_vypujcky, Datum_start, Datum_konec, Vraceno, ID_exemplare, ID_prukazu, ID_pracovnika)
VALUES (NULL, TO_DATE('2023-10-05', 'YYYY-MM-DD'), TO_DATE('2023-11-05', 'YYYY-MM-DD'), 'N', 1, 1001, 2001);

COMMIT;


-- ====================================================================
-- ===========       4. CAST: POKROCILE OBJEKTY        ===============
-- ====================================================================

-- ============ TRIGGER #1: zmena stavu exemplare pri vypujcce ============
CREATE OR REPLACE TRIGGER trg_exemplar_stav
AFTER INSERT ON Vypujcka
FOR EACH ROW
BEGIN
    UPDATE Exemplar SET Stav = 'Vypujceno' WHERE ID_exemplare = :NEW.ID_exemplare;
END;
/

-- ============ TRIGGER #2: validacia datumov vypujcky ============
CREATE OR REPLACE TRIGGER trg_vypujcka_dat
BEFORE INSERT OR UPDATE ON Vypujcka
FOR EACH ROW
BEGIN
    IF :NEW.Datum_konec <= :NEW.Datum_start THEN
        RAISE_APPLICATION_ERROR(-20001, 'Datum_konec musi byt po Datum_start');
    END IF;
END;
/

-- DEMO triggerov
SET SERVEROUTPUT ON;
INSERT INTO Vypujcka (ID_vypujcky, Datum_start, Datum_konec, Vraceno, ID_exemplare, ID_prukazu, ID_pracovnika)
VALUES (NULL, SYSDATE, SYSDATE+10, 'N', 3, 1001, 2001);
SELECT ID_exemplare, Stav FROM Exemplar WHERE ID_exemplare = 3;  -- 'Vypujceno' (trg #1)

BEGIN
    INSERT INTO Vypujcka (ID_vypujcky, Datum_start, Datum_konec, Vraceno, ID_exemplare, ID_prukazu, ID_pracovnika)
    VALUES (NULL, SYSDATE, SYSDATE-1, 'N', 3, 1001, 2001);
EXCEPTION WHEN OTHERS THEN DBMS_OUTPUT.PUT_LINE('Trg #2 zachytil: ' || SQLERRM);
END;
/
COMMIT;


-- ============ PROCEDURA 1: pokuty (KURZOR + %TYPE + EXCEPTION) ============
CREATE OR REPLACE PROCEDURE proc_pokuty(p_id_prukazu IN INT) AS
    CURSOR c IS
        SELECT v.ID_vypujcky, t.Nazev, (TRUNC(SYSDATE) - v.Datum_konec) AS dni
        FROM Vypujcka v
        JOIN Exemplar e ON v.ID_exemplare = e.ID_exemplare
        JOIN Titul    t ON e.ID_titulu    = t.ID_titulu
        WHERE v.ID_prukazu = p_id_prukazu
          AND v.Vraceno = 'N' AND v.Datum_konec < TRUNC(SYSDATE);
    v_jmeno  Uzivatel.Jmeno%TYPE;
    v_pokuta NUMBER := 0;
BEGIN
    SELECT u.Jmeno INTO v_jmeno
      FROM Uzivatel u JOIN Ctenar c ON u.ID_uzivatele = c.ID_uzivatele
     WHERE c.ID_prukazu = p_id_prukazu;
    DBMS_OUTPUT.PUT_LINE('Ctenar: ' || v_jmeno);
    FOR r IN c LOOP
        v_pokuta := v_pokuta + r.dni * 5;
        DBMS_OUTPUT.PUT_LINE('  ' || r.Nazev || ' - ' || r.dni || ' dni, pokuta ' || (r.dni*5) || ' Kc');
    END LOOP;
    DBMS_OUTPUT.PUT_LINE('Celkova pokuta: ' || v_pokuta || ' Kc');
EXCEPTION
    WHEN NO_DATA_FOUND THEN DBMS_OUTPUT.PUT_LINE('Ctenar neexistuje');
    WHEN OTHERS THEN DBMS_OUTPUT.PUT_LINE('Chyba: ' || SQLERRM);
END;
/

-- ============ PROCEDURA #2: report titulu (%ROWTYPE + EXCEPTION) ============
CREATE OR REPLACE PROCEDURE proc_report_titulu(p_id_titulu IN INT) AS
    v_titul Titul%ROWTYPE;
    v_pocet INT;
BEGIN
    SELECT * INTO v_titul FROM Titul WHERE ID_titulu = p_id_titulu;
    SELECT COUNT(*) INTO v_pocet FROM Exemplar WHERE ID_titulu = p_id_titulu;
    DBMS_OUTPUT.PUT_LINE(v_titul.Nazev || ' (' || v_titul.Rok_vydani || '): ' || v_pocet || ' exemplaru');
EXCEPTION
    WHEN NO_DATA_FOUND THEN DBMS_OUTPUT.PUT_LINE('Titul neexistuje');
END;
/

-- DEMO procedur
-- ma pozdnu vypujcku -> ukaze pokutu
EXEC proc_pokuty(1001);
-- vyjimka NO_DATA_FOUND
EXEC proc_pokuty(9999);
-- R.U.R., 2 exemplare
EXEC proc_report_titulu(1);


-- ============ INDEX + EXPLAIN PLAN (JOIN 3 tabuliek + COUNT + GROUP BY) ============
-- Navrh urychlenia: index na FK Vypujcka.ID_prukazu umozni pri JOIN-e
-- pouzit INDEX RANGE SCAN namiesto FULL TABLE SCAN nad Vypujcka.
EXPLAIN PLAN FOR
SELECT u.Prijmeni, COUNT(v.ID_vypujcky) AS pocet
FROM Uzivatel u
JOIN Ctenar   c ON u.ID_uzivatele = c.ID_uzivatele
JOIN Vypujcka v ON c.ID_prukazu   = v.ID_prukazu
GROUP BY u.Prijmeni;
SELECT * FROM TABLE(DBMS_XPLAN.DISPLAY);

CREATE INDEX idx_vypujcka_prukaz ON Vypujcka(ID_prukazu);

EXPLAIN PLAN FOR
SELECT u.Prijmeni, COUNT(v.ID_vypujcky) AS pocet
FROM Uzivatel u
JOIN Ctenar   c ON u.ID_uzivatele = c.ID_uzivatele
JOIN Vypujcka v ON c.ID_prukazu   = v.ID_prukazu
GROUP BY u.Prijmeni;
SELECT * FROM TABLE(DBMS_XPLAN.DISPLAY);


-- ============ PRISTUPOVA PRAVA pre xderned00 (druhy clen tymu) ============
GRANT SELECT, INSERT, UPDATE, DELETE ON Uzivatel    TO xderned00;
GRANT SELECT, INSERT, UPDATE, DELETE ON Ctenar      TO xderned00;
GRANT SELECT, INSERT, UPDATE, DELETE ON Pracovnik   TO xderned00;
GRANT SELECT, INSERT, UPDATE, DELETE ON Autor       TO xderned00;
GRANT SELECT, INSERT, UPDATE, DELETE ON Titul       TO xderned00;
GRANT SELECT, INSERT, UPDATE, DELETE ON Kniha       TO xderned00;
GRANT SELECT, INSERT, UPDATE, DELETE ON Casopis     TO xderned00;
GRANT SELECT, INSERT, UPDATE, DELETE ON Titul_Autor TO xderned00;
GRANT SELECT, INSERT, UPDATE, DELETE ON Exemplar    TO xderned00;
GRANT SELECT, INSERT, UPDATE, DELETE ON Rezervace   TO xderned00;
GRANT SELECT, INSERT, UPDATE, DELETE ON Vypujcka    TO xderned00;
GRANT EXECUTE ON proc_pokuty        TO xderned00;
GRANT EXECUTE ON proc_report_titulu TO xderned00;


-- ============ MATERIALIZOVANY POHLED (vlastnik xderned00, pouziva tabulky xdorna06) ============
-- POZN: tato cast je urcena pre druheho clena tymu (xderned00), ktory ma vyssimi
-- GRANTami pristup k tabulkam xdorna06. Pri spustani celeho skriptu pod uctom
-- xdorna06 sa MV vytvori v jeho schéme, ale syntaxe (xdorna06.Titul atd.)
-- demonstruje cross-schema pristup popsany v zadani.
CREATE MATERIALIZED VIEW mv_statistika_titulu
BUILD IMMEDIATE REFRESH COMPLETE ON DEMAND AS
SELECT t.ID_titulu, t.Nazev, COUNT(v.ID_vypujcky) AS pocet_vypujcek
FROM xdorna06.Titul t
LEFT JOIN xdorna06.Exemplar e ON t.ID_titulu    = e.ID_titulu
LEFT JOIN xdorna06.Vypujcka v ON e.ID_exemplare = v.ID_exemplare
GROUP BY t.ID_titulu, t.Nazev;

SELECT * FROM mv_statistika_titulu;
INSERT INTO xdorna06.Vypujcka (ID_vypujcky, Datum_start, Datum_konec, Vraceno, ID_exemplare, ID_prukazu, ID_pracovnika)
VALUES (NULL, SYSDATE, SYSDATE+10, 'N', 1, 1001, 2001);
COMMIT;
SELECT * FROM mv_statistika_titulu;                  -- stale stary snapshot
EXEC DBMS_MVIEW.REFRESH('mv_statistika_titulu');
SELECT * FROM mv_statistika_titulu;                  -- po refresh aktualne data


-- ============ KOMPLEXNY DOTAZ: WITH + CASE ============
-- Ziska prehlad ctenarov s celkovym poctom vypozicok a kategorizuje
-- ich (Novy/Obcasny/Pravidelny) plus oznaci tych s pozdnymi vypozickami.
WITH stat AS (
    SELECT c.ID_prukazu, u.Prijmeni,
           COUNT(v.ID_vypujcky) AS celkem,
           SUM(CASE WHEN v.Vraceno='N' AND v.Datum_konec < TRUNC(SYSDATE) THEN 1 ELSE 0 END) AS pozdne
    FROM Ctenar c
    JOIN Uzivatel u ON c.ID_uzivatele = u.ID_uzivatele
    LEFT JOIN Vypujcka v ON c.ID_prukazu = v.ID_prukazu
    GROUP BY c.ID_prukazu, u.Prijmeni
)
SELECT Prijmeni, celkem, pozdne,
       CASE WHEN celkem = 0  THEN 'Novy'
            WHEN celkem <= 2 THEN 'Obcasny'
            ELSE                  'Pravidelny' END AS kategorie,
       CASE WHEN pozdne > 0 THEN 'POZOR pokuty' ELSE 'OK' END AS status
FROM stat
ORDER BY celkem DESC;