Feature: User behaviour
  Scenario: Create new user
    Given I am logged in as admin
    When I navigate to user's page
    And create new user with member role
    Then The new user is visible in the user table

  Scenario: Relog as new user
    Given a new created user exists
    When I Log in with new user's credentials
    Then I am loged in as new user

  Scenario: Change password
    Given I am on my account page
    When I fill in two different passwords
    Then I won't be able to save the new password
    When I fill in identical passwords
    Then changed password dialog appears

  Scenario: Log in with new password
    Given I changed password
    When I log out
    And I try to log in with old password
    Then I won't log in
    When I type in new credentials
    Then I log in as the new user

  Scenario: Delete user
    Given I am logged as admin on users page
    When I delete new account
    Then the account doesn't exist in the table
    Then I cannot log as this user anymore