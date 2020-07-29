# Access Policy
Access Policies are used by the device owner to express under which circumstances his devices will be accessed, and by whom.

The Access Policy Language is instance of the language [PBel](http://www.doc.ic.ac.uk/~mrh/talks/BelnapTalk.pdf) (pronounced “pebble”). A basic form of policy is captured in a rule. There are two simple types of rules from which more complex policies can be formed:

```
grant if cond
deny if !cond
```
where `cond` are logical expressions built from attributes, their values and comparisons, as well as logical operators.

For example, we may specify an access-control rule:

```
grant if (object == vehicle) && (subject == vehicle.owner.daughter) &&
         (action == driveVehicle) &&
         (0900 ≤ localTime) && (localTime ≤ 2000)
```

This rule implicitly refers to the request made by the daughter of the owner of the vehicle to drive that car. This rule applies only if the requested resource is that vehicle, the action is to drive that vehicle, and the requester is the daughter of the owner of that vehicle. In those circumstances, access is granted if the local time is between 9am and 8pm. The intuition is that this rule does not apply whenever its condition `cond` evaluates to `false`, including in cases in which the request is not of that type.
