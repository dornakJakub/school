import * as tools from './tools'

describe('Users features', () => {
    let data;
    var unique;

    beforeEach(() => {
        cy.fixture('general').then((d) => {
        data = d;
        unique = '_' + Date.now().toString();
        cy.visit('http://localhost:8000/login');
        cy.login(data.admin);
        tools.navigateToUsers();
        cy.get('#new-user-email').type(data.testUser.email);
        cy.get('#new-user-password').type(data.testUser.password);
        cy.get('.form-check').contains('MEMBER').get('.form-check-input').check();
        tools.clickButton('Vytvořit uživatele');
        tools.tableContains(data.testUser.email);
        })
    });

    afterEach(function () {
        cy.logout();
        cy.login(data.admin);
        tools.navigateToUsers();
        tools.clickButtonOnRow(data.testUser.email, 'Smazat');
        tools.tableDoesntContain(data.testUser.email);
        cy.logout();
    });

    it ('Create new user', () => {
        //Create and delete a user by beforeEach() and afterEach()
    })

    it ('Log as new user', () => {
        cy.logout();
        cy.login(data.testUser);
    })

    it ('Change password', () => {
        cy.logout();
        cy.login(data.testUser);
        tools.navigateToAccount();
        cy.get('#account-new-password').clear().type(data.testUser.newPassword);
        cy.get('#account-confirm-password').clear().type(data.testUser.wrongPassword);
        cy.get('.btn').contains('Uložit heslo').should('be.disabled');
        cy.get('#account-new-password').clear().type(data.testUser.newPassword);
        cy.get('#account-confirm-password').clear().type(data.testUser.newPassword);
        tools.clickButton('Uložit heslo');
    })

    it ('Log in with new password', () => {
        cy.logout();
        cy.login(data.testUser);
        tools.navigateToAccount();
        cy.get('#account-new-password').clear().type(data.testUser.newPassword);
        cy.get('#account-confirm-password').clear().type(data.testUser.newPassword);
        tools.clickButton('Uložit heslo');

        cy.logout();
        cy.get('#login-email').type(data.testUser.email);
        cy.get('#login-password').type(data.testUser.password);
        tools.clickButton('Přihlásit se');
        cy.login(data.testUser.newLogin);
    })
})