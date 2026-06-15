import * as tools from './tools'

describe('Users features', () => {
    let data;

    beforeEach(() => {
        cy.fixture('general').then((d) => {
        data = d;
        cy.visit('http://localhost:8000/login');
        cy.login(data.admin);
        cy.createMember(data.testMember);
        })
    });

    afterEach(function () {
        tools.navigateToMembers();
        cy.deleteMember(data.testMember);
        cy.logout();
    });

    it ('Create awaited payment', () => {
        tools.checkTableRow(data.testMember.name);
        tools.clickButton('Připravit očekávanou platbu');
        cy.get('#prepare-title').type(data.testPayment.amount);
        cy.get('#prepare-amount').type(data.testPayment.amount);
        tools.clickButton('Vytvořit očekávané platby');
        tools.navigateToPayments();
        tools.checkValueOnRow(data.testMember.name, data.testPayment.amount);
    })

    it ('Pair payment with matching transaction', () => {
        tools.checkTableRow(data.testMember.name);
        tools.clickButton('Připravit očekávanou platbu');
        cy.get('#prepare-title').type(data.testPayment.amount);
        cy.get('#prepare-amount').type(data.testPayment.amount);
        tools.clickButton('Vytvořit očekávané platby');
        tools.navigateToPayments();
        tools.checkValueOnRow(data.testMember.name, data.testPayment.amount);

        tools.navigateToOperations();
        cy.get('#payments-manual-amount').type(data.testPayment.amount);
        const todayDate = new Date().toISOString().split('T')[0];
        cy.get('#payments-manual-date').type(todayDate);
        tools.clickButton('Zadat ruční transakci');

        tools.selectFromCombobox('#payments-match-tx', data.testPayment.amount + '.00');
        tools.selectFromCombobox('#payments-match-payment', data.testMember.name);
        tools.clickButton('Spárovat platbu');
        tools.checkValueOnRow(data.testMember.name, data.testPayment.amount);
    })

    it ('Pair payment with incorrect amount', () => {
        tools.checkTableRow(data.testMember.name);
        tools.clickButton('Připravit očekávanou platbu');
        cy.get('#prepare-title').type(data.testPayment.amount);
        cy.get('#prepare-amount').type(data.testPayment.amount);
        tools.clickButton('Vytvořit očekávané platby');
        tools.navigateToPayments();
        tools.checkValueOnRow(data.testMember.name, data.testPayment.amount);

        tools.navigateToOperations();
        cy.get('#payments-manual-amount').type(data.testPayment.wrongAmount);
        const todayDate = new Date().toISOString().split('T')[0];
        cy.get('#payments-manual-date').type(todayDate);
        tools.clickButton('Zadat ruční transakci');

        tools.selectFromCombobox('#payments-match-tx', data.testPayment.wrongAmount + '.00');
        tools.selectFromCombobox('#payments-match-payment', data.testMember.name);
        tools.clickButton('Spárovat platbu');
        tools.checkValueOnRowNot(data.testMember.name, data.testPayment.amount);
    })
});