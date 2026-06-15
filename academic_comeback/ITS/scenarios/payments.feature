Feature: Payment pairing
  Scenario: Create awaited payment
    Given I am logged in as committee
    And a member exists
    When I create a payment request for the member with amount 100
    Then the payment is listed as awaited

  Scenario: Pair payment with matching transaction
    Given awaited payment exists with amount 100
    And I am logged in as admin
    When I pair it with transaction of amount 100
    Then the payment is marked as paired

  Scenario: Pair payment with incorrect amount
    Given awaited payment exists with amount 100
    When I try to pair it with transaction of amount 50
    Then the pairing is rejected
  
