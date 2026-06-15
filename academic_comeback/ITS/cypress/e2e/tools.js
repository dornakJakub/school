export const clickButton = (buttonText) => {
    cy.get('.btn').contains(buttonText).click();
};

const navigateMenu = (buttonText) => {
    cy.get('.nav-link').contains(buttonText).click();
}

export const navigateToMembers = () => {
    navigateMenu('Členové');
}

export const navigateToUsers = () => {
    navigateMenu('Uživatelé');
}

export const navigateToPayments = () => {
    navigateMenu('Platby');
}

export const navigateToOperations = () => {
    navigateMenu('Operace plateb');
}

export const navigateToTeams = () => {
    navigateMenu('Týmy');
}

export const navigateToAccount = () => {
    navigateMenu('Můj účet');
}

export const tableContains = (text) => {
    cy.get('.table').contains(text).should('exist');
}

export const tableDoesntContain = (text) => {
    cy.get('.table').contains(text).should('not.exist');
}

export const tableClick = (text) => {
    cy.get('.table').contains(text).click();
}

export const setTableFilter = (filterName, text) => {
    cy.get('.champ-filter-input').contains('label', filterName).type(text);
}

export const checkTableLength = (len) => {
    cy.get('table tbody tr').should('have.length', len);
}

export const clickButtonOnRow = (value, btn) => {
    cy.contains('table tbody tr', value)
    .within(() => {
        cy.get('.btn').contains(btn).click();
    });
}

export const checkTableRow = (rowText) => {
    cy.contains('table tbody tr', rowText)
    .within(() => {
        cy.get('.form-check-input').check();
    });
}

export const checkValueOnRow = (rowText, value) => {
    cy.contains('table tbody tr', rowText)
    .within(() => {
        cy.contains(value).should('exist');
    });
}

export const checkValueOnRowNot = (rowText, value) => {
    cy.contains('table tbody tr', rowText)
    .within(() => {
        cy.contains(value).should('not.exist');
    });
}

export const selectFromCombobox = (combobox, value) => {
    cy.get(combobox)
        .contains(value)
        .invoke('val')
        .then(value => {
            cy.get(combobox).select(value)
    })
}

export const clickValueInTable = (value) => {
    cy.contains('table tbody tr', value).click();
}