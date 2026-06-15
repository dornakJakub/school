// ***********************************************
// This example commands.js shows you how to
// create various custom commands and overwrite
// existing commands.
//
// For more comprehensive examples of custom
// commands please read more here:
// https://on.cypress.io/custom-commands
// ***********************************************
//
//
// -- This is a parent command --
// Cypress.Commands.add('login', (email, password) => { ... })
//
//
// -- This is a child command --
// Cypress.Commands.add('drag', { prevSubject: 'element'}, (subject, options) => { ... })
//
//
// -- This is a dual command --
// Cypress.Commands.add('dismiss', { prevSubject: 'optional'}, (subject, options) => { ... })
//
//
// -- This will overwrite an existing command --
// Cypress.Commands.overwrite('visit', (originalFn, url, options) => { ... })

import * as tools from './../e2e/tools' 

Cypress.Commands.add('login', (credentials) => {
    cy.get('#login-email').clear().type(credentials.email);
    cy.get('#login-password').clear().type(credentials.password);
    tools.clickButton('Přihlásit se');

    cy.get('.champ-shell__header').should('be.visible');
})

Cypress.Commands.add('logout', () => {
    tools.clickButton('Odhlásit se');
})

Cypress.Commands.add('createMember', (memberData, unique = '') => {
    tools.clickButton('Přidat nového člena');

    cy.get('#new-member-first-name').type(memberData.name + unique);
    cy.get('#new-member-last-name').type(memberData.surname);

    tools.clickButton('Přidat člena');
    
    tools.navigateToMembers();
    tools.tableContains(memberData.name + unique);
    tools.tableContains(memberData.surname);
})

Cypress.Commands.add('deleteMember', (memberData) => {
    tools.tableClick(memberData.name);
    tools.clickButton('Trvale odstranit člena');
})

Cypress.Commands.add('createUser', (userData) => {
    cy.get('#new-user-email').type(userData.email);
    cy.get('#new-user-password').type(userData.password);
    tools.clickButton('Vytvořit uživatele');
})