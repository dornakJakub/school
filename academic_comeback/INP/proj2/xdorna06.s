; Autor reseni: jakub dornak xdorna06

; Projekt 2 - INP 2025
; Souhlaskove modulovana samohlaskova sifra na architekture MIPS64

; DATA SEGMENT
                .data
msg:            .asciiz "jakubdornak" ; sem doplnte vase "jmenoprijmeni"
cipher:         .space  31 ; misto pro zapis zasifrovaneho textu
; zde si muzete nadefinovat vlastni promenne ci konstanty,
; napr. hodnoty posuvu pro jednotlive znaky sifrovacho klice

params_sys5:    .space  8 ; misto pro ulozeni adresy pocatku
                          ; retezce pro vypis pomoci syscall 5
                          ; (viz nize "funkce" print_string)

; CODE SEGMENT
                .text

main:           ; ZDE NAHRADTE KOD VASIM RESENIM
                daddi   r3, r0, 26          ; inicializace hodnoty klice pro sifrovani
                daddi   r1, r0, msg         ; vozrovy vypis: adresa msg do r4

loop:
                lb      r2, 0(r1)           ; nacti aktualni znak do r2
                beq     r2, r0, finish   ; pokud je aktualni znak ukoncovaci, skoc na sekci finish
                daddi   r4, r0, 97          ; nacti hodnotu pro 'a' do r4
                beq     r2, r4, is_vowel    ; pokud se nacteny znak rovna 'a', zpracuj jako samohlasku
                daddi   r4, r0, 101         ; nacti hodnotu pro 'e' do r4
                beq     r2, r4, is_vowel    ; pokud se nacteny znak rovna 'e', zpracuj jako samohlasku
                daddi   r4, r0, 105         ; nacti hodnotu pro 'i' do r4
                beq     r2, r4, is_vowel    ; pokud se nacteny znak rovna 'i', zpracuj jako samohlasku
                daddi   r4, r0, 111         ; nacti hodnotu pro 'o' do r4
                beq     r2, r4, is_vowel    ; pokud se nacteny znak rovna 'o', zpracuj jako samohlasku
                daddi   r4, r0, 117         ; nacti hodnotu pro 'u' do r4
                beq     r2, r4, is_vowel    ; pokud se nacteny znak rovna 'u', zpracuj jako samohlasku
                daddi   r4, r0, 121         ; nacti hodnotu pro 'y' do r4
                beq     r2, r4, is_vowel    ; pokud se nacteny znak rovna 'y', zpracuj jako samohlasku

                ; aktualni znak neni samohlaska
                sb      r2, 0(r5)           ; ulozit aktualni znak beze zmeny do vysledneho retezce
                addi    r5, r5, 1           ; posun pozice pro zapisovani
                addi    r1, r1, 1           ; posun pozice pro cteni

                ; vypocet nove hodnoty klice pro sifrovani
                daddi   r4, r0, 97          ; nacti 'a' do r4
                sub     r3, r2, r4          ; r3 = r2 - r4 (0 .. 25)
                addi    r3, r3, 1           ; (1 .. 26)

                j       loop                ; skok zpet na zacatek cyklu cteni znaku
is_vowel:
                ; zpracovani detekovane samohlasky
                daddi   r9, r0, 97          ; nahrat 'a' do r9
                sub     r7, r2, r9          ; r7 = char_index (0 .. 25)
                addu    r7, r7, r3          ; r7 = char_index + klic
                daddi   r6, r0, 26          ; nacist 26 do r6 pro porovnavani
                sub     r8, r7, r6          ; r8 = r7 - r6
                bgez    r8, store_shifted   ; pokud char_index pretekl pres 26, skoc na opravu overflow
                daddi   r8, r7, 0           ; nacteni znaku do registru pro ulozeni

store_shifted:
                ; ulozeni samohlasky
                add     r8, r8, r9          ; prevedeni char_indexu v abecede na ascii znak
                sb      r8, 0(r5)           ; ulozeni sifrovaneho znaku do vysledneho retezce
                addi    r5, r5, 1           ; posun pozice pro zapisovani
                addi    r1, r1, 1           ; posun pozice pro cteni
                j	    loop                ; skok zpet na zacatek cyklu cteni znaku

overflow:
                ; oprava preteceni sifrovane samohlasky
                subu    r8, r8, r6          ; oprava preteceni
                j       store_shifted       ; skok na ulozeni samohlasky    

finish:         ; priprava zakodovane zpravy pro tisk
                daddi   r4, r0, msg         ; nacteni adresy retezce pro vypis
                jal     print_string        ; vypis pomoci print_string


; NASLEDUJICI KOD NEMODIFIKUJTE!

                syscall 0   ; halt

print_string:   ; adresa retezce se ocekava v r4
                sw      r4, params_sys5(r0)
                daddi   r14, r0, params_sys5    ; adr pro syscall 5 musi do r14
                syscall 5   ; systemova procedura - vypis retezce na terminal
                jr      r31 ; return - r31 je urcen na return address
