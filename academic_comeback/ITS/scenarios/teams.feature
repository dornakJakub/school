Feature: Team with members
    Scenario: Add new team
    Given I am logged in
    When I navigate to teams page
    And I fill form for creating new team and add that team
    Then the team exists in the team table with 0 members

  Scenario: Add member to team
    Given team exists and member exists
    When I open member details
    And assign member to the new team
    Then the team has more members

  Scenario: Edit team
    Given I am on the team page and team exists
    When I click the edit button of the new team
    And Change info
    And click "Enter"
    Then the team info has been updated

  Scenario: Delete team
    Given I am on teams page and team exists
    When I select a team from the table and delete it
    Then the team no longer exists in the table


