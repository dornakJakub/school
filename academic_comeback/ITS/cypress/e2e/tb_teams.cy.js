import * as tools from './tools'

describe('Users features', () => {
    let data;
    var unique;

    beforeEach(() => {
        unique = '_' + Date.now().toString();
        cy.fixture('general').then((d) => {
        data = d;
        cy.visit('http://localhost:8000/login');
        cy.login(data.admin);
        cy.createMember(data.testMember, unique);
        })
    });

    afterEach(function () {
        tools.navigateToTeams();
        tools.clickButtonOnRow(data.testTeam.name, 'Smazat');
        cy.on('window:confirm', () => true)
        tools.navigateToMembers();
        cy.deleteMember(data.testMember);
        cy.logout();
    });

    it ('Add new team', () => {
        tools.navigateToTeams();
        cy.get('#new-team-name').type(data.testTeam.name + unique);
        tools.clickButton('Přidat tým');
        tools.checkValueOnRow(data.testTeam.name, '0');
    })

    it ('Add member to team', () => {
        tools.navigateToTeams();
        cy.get('#new-team-name').type(data.testTeam.name + unique);
        tools.clickButton('Přidat tým');
        tools.checkValueOnRow(data.testTeam.name + unique, '0');

        tools.navigateToMembers();
        tools.clickValueInTable(data.testMember.name + unique);
        cy.get('.table .form-select').select(data.testTeam.name + unique);
        const today = new Date();
        const todayDate = today.toISOString().split('T')[0];
        cy.get('.table .form-control').eq(0).type(todayDate);
        cy.get('.table .form-control').eq(1).type(todayDate);
        tools.clickButton('Uložit člena');

        tools.navigateToTeams();
        tools.checkValueOnRow(data.testTeam.name + unique, '1');
    })

    it ('Edit team', () => {
        tools.navigateToTeams();
        cy.get('#new-team-name').type(data.testTeam.name + unique);
        cy.get('#new-team-note').type(data.testTeam.note);
        tools.clickButton('Přidat tým');

        tools.clickButtonOnRow(data.testTeam.name + unique, 'Upravit');
        cy.get(':nth-child(3) > .form-control').clear().type(data.testTeam.newNote);
        tools.clickButton('Potvrdit');

        tools.checkValueOnRow(data.testTeam.name + unique, data.testTeam.newNote);
    })
});