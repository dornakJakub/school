Feature: Members management
  Scenario: Add new Member
    Given I am logged in as admin
    When I Add new member using add member form
    Then new member is visible in member list

  Scenario: Filter user
    Given  I am on members page
    When filtering new member name
    Then only new member is visible
    When clearing filters
    Then all members are visible

  Scenario: Delete member
    Given I am on members page
    When I open member details 
    And click Delete
    Then the member does not exist in the member table

