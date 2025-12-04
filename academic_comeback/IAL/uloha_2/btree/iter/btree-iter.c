/*
 * Binární vyhledávací strom — iterativní varianta
 *
 * S využitím datových typů ze souboru btree.h, zásobníku ze souboru stack.h
 * a připravených koster funkcí implementujte binární vyhledávací
 * strom bez použití rekurze.
 */

#include "../btree.h"
#include "stack.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Inicializace stromu.
 *
 * Uživatel musí zajistit, že inicializace se nebude opakovaně volat nad
 * inicializovaným stromem. V opačném případě může dojít k úniku paměti (memory
 * leak). Protože neinicializovaný ukazatel má nedefinovanou hodnotu, není
 * možné toto detekovat ve funkci.
 */
void bst_init(bst_node_t **tree)
{
    if (tree == NULL) return;
  *tree = NULL;
}

/*
 * Vyhledání uzlu v stromu.
 *
 * V případě úspěchu vrátí funkce hodnotu true a do proměnné value zapíše
 * ukazatel na obsah daného uzlu. V opačném případě funkce vrátí hodnotu false a proměnná
 * value zůstává nezměněná.
 *
 * Funkci implementujte iterativně bez použité vlastních pomocných funkcí.
 */
bool bst_search(bst_node_t *tree, char key, bst_node_content_t **value)
{
  if (tree == NULL)
    return false;

  bst_node_t *node = tree;
  while (node != NULL) {
    if (node->key == key) {
      *value = &node->content;
      return true;
    }

    if (node->key > key)
      node = node->left;
    else
      node = node->right;
  }

  return false;
}

/*
 * Vložení uzlu do stromu.
 *
 * Pokud uzel se zadaným klíče už ve stromu existuje, nahraďte jeho hodnotu.
 * Jinak vložte nový listový uzel. V případě úspěchu funkce tuto skutečnost
 * indikuje návratovou hodnotou true, v opačném případě (např. při selhání 
 * alokace) vrací funkce false.
 *
 * Výsledný strom musí splňovat podmínku vyhledávacího stromu — levý podstrom
 * uzlu obsahuje jenom menší klíče, pravý větší.
 *
 * Funkci implementujte iterativně bez použití vlastních pomocných funkcí.
 */
bool bst_insert(bst_node_t **tree, char key, bst_node_content_t value)
{
  bst_node_t **curr = tree;

  while (*curr != NULL) {
      if (key == (*curr)->key) {
        free((*curr)->content.value);
        (*curr)->content.value = value.value;
        (*curr)->content.type = value.type;
        return true;
      } else if (key < (*curr)->key)
        curr = &(*curr)->left;
      else
        curr = &(*curr)->right;
  }

  bst_node_t *new_node = malloc(sizeof(bst_node_t));
  if (new_node == NULL)
    return false;

  new_node->key = key;
  new_node->content.value = value.value;
  new_node->content.type = value.type;
  new_node->left = NULL;
  new_node->right = NULL;

  *curr = new_node;
  return true;
}

/*
 * Pomocná funkce která nahradí uzel nejpravějším potomkem.
 *
 * Klíč a hodnota uzlu target budou nahrazené klíčem a hodnotou nejpravějšího
 * uzlu podstromu tree. Nejpravější potomek bude odstraněný. Funkce korektně
 * uvolní všechny alokované zdroje odstraněného uzlu.
 *
 * Funkce předpokládá, že hodnota tree není NULL.
 *
 * Tato pomocná funkce bude využita při implementaci funkce bst_delete.
 *
 * Funkci implementujte iterativně bez použití vlastních pomocných funkcí.
 */
void bst_replace_by_rightmost(bst_node_t *target, bst_node_t **tree)
{
    bst_node_t **curr = tree;
    bst_node_t *node = *tree;

    while (node->right != NULL) {
      curr = &node->right;
      node = node->right;
    }

    target->key = node->key;
    free(target->content.value);
    target->content.value = node->content.value;
    target->content.type = node->content.type;

    bst_node_t *subtree = node->left;
    free(node);
    *curr = subtree;
}

/*
 * Odstranění uzlu ze stromu.
 *
 * Pokud uzel se zadaným klíčem neexistuje, funkce nic nedělá.
 * Pokud má odstraněný uzel jeden podstrom, zdědí ho rodič odstraněného uzlu.
 * Pokud má odstraněný uzel oba podstromy, je nahrazený nejpravějším uzlem
 * levého podstromu. Nejpravější uzel nemusí být listem.
 *
 * Funkce korektně uvolní všechny alokované zdroje odstraněného uzlu.
 *
 * Funkci implementujte iterativně pomocí bst_replace_by_rightmost a bez
 * použití vlastních pomocných funkcí.
 */
void bst_delete(bst_node_t **tree, char key)
{
  bst_node_t **curr = tree;

  while (*curr != NULL) {
    if (key == (*curr)->key) {
      bst_node_t *node = *curr;
      if (node->left != NULL && node->right != NULL)
        bst_replace_by_rightmost(node, &(node->left));
      else if (node->left != NULL) {
        *curr = node->left;
        free(node->content.value);
        free(node);
      } else if (node->right != NULL) {
        *curr = node->right;
        free(node->content.value);
        free(node);
      } else {
        free(node->content.value);
        free(node);
        *curr = NULL;
      }
      return;
      
    } else if (key < (*curr)->key)
      curr = &(*curr)->left;
    else
      curr = &(*curr)->right;
  }
}

/*
 * Zrušení celého stromu.
 *
 * Po zrušení se celý strom bude nacházet ve stejném stavu jako po
 * inicializaci. Funkce korektně uvolní všechny alokované zdroje rušených
 * uzlů.
 *
 * Funkci implementujte iterativně s pomocí zásobníku a bez použití
 * vlastních pomocných funkcí.
 */
void bst_dispose(bst_node_t **tree)
{
  if (*tree == NULL)
    return;

  stack_bst_t stack;
  stack_bst_init(&stack);
  stack_bst_push(&stack, *tree);

  while (!stack_bst_empty(&stack)) {
    bst_node_t *node = stack_bst_pop(&stack);

    if (node->left != NULL)
      stack_bst_push(&stack, node->left);

    if (node->right != NULL)
      stack_bst_push(&stack, node->right);
    free(node->content.value);
    free(node);
    node = NULL;
  }

  *tree = NULL;
}

/*
 * Pomocná funkce pro iterativní preorder.
 *
 * Prochází po levé větvi k nejlevějšímu uzlu podstromu.
 * Nad zpracovanými uzly zavolá bst_add_node_to_items a uloží je do zásobníku uzlů.
 *
 * Funkci implementujte iterativně s pomocí zásobníku a bez použití
 * vlastních pomocných funkcí.
 */
void bst_leftmost_preorder(bst_node_t *tree, stack_bst_t *to_visit, bst_items_t *items)
{
    bst_node_t *node = tree;

    while (node != NULL) {
        bst_add_node_to_items(node, items);

        stack_bst_push(to_visit, node);

        node = node->left;
    }
}

/*
 * Preorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte iterativně pomocí funkce bst_leftmost_preorder a
 * zásobníku uzlů a bez použití vlastních pomocných funkcí.
 */
void bst_preorder(bst_node_t *tree, bst_items_t *items)
{
  if (tree == NULL)
    return;

  stack_bst_t to_visit;
  stack_bst_init(&to_visit);

  bst_leftmost_preorder(tree, &to_visit, items);

  while (!stack_bst_empty(&to_visit)) {
    bst_node_t *node = stack_bst_pop(&to_visit);

    if (node->right != NULL)
      bst_leftmost_preorder(node->right, &to_visit, items);
  }
}

/*
 * Pomocná funkce pro iterativní inorder.
 *
 * Prochází po levé větvi k nejlevějšímu uzlu podstromu a ukládá uzly do
 * zásobníku uzlů.
 *
 * Funkci implementujte iterativně s pomocí zásobníku a bez použití
 * vlastních pomocných funkcí.
 */
void bst_leftmost_inorder(bst_node_t *tree, stack_bst_t *to_visit)
{
  bst_node_t *node = tree;

  while (node != NULL) {
    stack_bst_push(to_visit, node);

    node = node->left;
  }
}

/*
 * Inorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte iterativně pomocí funkce bst_leftmost_inorder a
 * zásobníku uzlů a bez použití vlastních pomocných funkcí.
 */
void bst_inorder(bst_node_t *tree, bst_items_t *items)
{
  if (tree == NULL)
    return;

  stack_bst_t to_visit;
  stack_bst_init(&to_visit);

  bst_leftmost_inorder(tree, &to_visit);

  while (!stack_bst_empty(&to_visit)) {
    bst_node_t *node = stack_bst_pop(&to_visit);
    bst_add_node_to_items(node, items);

    if (node->right != NULL)
      bst_leftmost_inorder(node->right, &to_visit);
  }
}

/*
 * Pomocná funkce pro iterativní postorder.
 *
 * Prochází po levé větvi k nejlevějšímu uzlu podstromu a ukládá uzly do
 * zásobníku uzlů. Do zásobníku bool hodnot ukládá informaci, že uzel
 * byl navštíven poprvé.
 *
 * Funkci implementujte iterativně pomocí zásobníku uzlů a bool hodnot a bez použití
 * vlastních pomocných funkcí.
 */
void bst_leftmost_postorder(bst_node_t *tree, stack_bst_t *to_visit,
                            stack_bool_t *first_visit)
{
  bst_node_t *node = tree;

  while (node != NULL) {
    stack_bst_push(to_visit, node);
    stack_bool_push(first_visit, true);
    node = node->left;
  }
}

/*
 * Postorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte iterativně pomocí funkce bst_leftmost_postorder a
 * zásobníku uzlů a bool hodnot a bez použití vlastních pomocných funkcí.
 */
void bst_postorder(bst_node_t *tree, bst_items_t *items)
{
  if (tree == NULL)
    return;

  stack_bst_t to_visit;
  stack_bst_init(&to_visit);
  stack_bool_t firstVisit;
  stack_bool_init(&firstVisit);

  bst_leftmost_postorder(tree, &to_visit, &firstVisit);

  while (!stack_bst_empty(&to_visit)) {
    bst_node_t *node = stack_bst_pop(&to_visit);
    bool isFirst = stack_bool_pop(&firstVisit);

    if (isFirst) {
      stack_bst_push(&to_visit, node);
      stack_bool_push(&firstVisit, false);

      if (node->right != NULL)
        bst_leftmost_postorder(node->right, &to_visit, &firstVisit);
    } else {
      bst_add_node_to_items(node, items);
    }
  }
}
