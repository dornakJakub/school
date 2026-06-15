import * as tools from './tools'

describe('Members', () => {

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

  it ('Add new member', () => {
    
  })

  it ('Member filter test', () => {
    tools.setTableFilter('Jméno', data.testMember.name);
    tools.setTableFilter('Příjmení', data.testMember.surname);
    
    tools.tableContains(data.testMember.name);
    tools.checkTableLength(1);

    tools.setTableFilter('Jméno', '');
    tools.setTableFilter('Příjmení', '');

    cy.get('table tbody tr').should('have.length.greaterThan', 1);
  })

})  