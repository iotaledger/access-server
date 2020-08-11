# Access Policy Specification

## Overview

Access Policies are used by the device owner to express under which circumstances his devices will be accessed, and by whom.

The Access Policy Language may be seen as an instance of the language [PBel](http://www.doc.ic.ac.uk/~mrh/talks/BelnapTalk.pdf) (pronounced “pebble”).

In the 1970s, Nuel Belnap suggested the use of a four-valued logic where ordinary truth values are used for truth and falsity. It also encodes a third truth value that expresses lack of knowledge. A fourth truth value expresses inconsistent knowledge. Belnap developed a semantics and a sound and complete Hilbert style proof system for this logic. Belnap logic extends naturally to first- and higher-order logics.

More recently, Michael Huth and Glenn Bruns have been researching the applications of [Access Control via Belnap Logic](https://www.doc.ic.ac.uk/research/technicalreports/2011/DTR11-6.pdf).

IOTA Access is a derivation of Michael Huth's previous work called FROST.

 A basic form of policy is captured in a rule and encoded in terms of `subject`, `object` and `action`, which are all forms of attributes. Two simple types of rules are proposed from which more complex policies can be formed:

 - `grant if cond`
 - `deny if cond`

where `cond` is a logical expression built from attributes, their values and comparisons, as well as logical operators.

For example, we may specify an access-control rule:
 ```
grant if (object == vehicle) && (subject == vehicle.owner.daughter) &&
         (action == driveVehicle) &&
         (owner.daughter.isInsured == true) &&
         (0900 ≤ localTime) && (localTime ≤ 2000)
```

This rule implicitly refers to the request made by the daughter of the owner of the vehicle to drive that car. This rule applies only if the requested resource is that vehicle, the action is to drive that vehicle, and the requester is the daughter of the owner of that vehicle. In those circumstances, access is granted if the daughter is insured and the local time is between 9am and 8pm. The intuition is that this rule does not apply whenever its condition `cond` evaluates to `false`, including in cases in which the request is not of that type.

![drawing](/docs/images/policy.png)

The language does not proscribe the semantics of attributes. For example, the attribute `owner.daughter` may be evaluated through a multi-factor authentication method whereas `daughter.insured` may consult the state of a smart contract.

## Policy Composition in Distributed Environments

In open and distributed systems (such as the IOTA Tangle), rules may not always apply to a request. This suggests that we want support for a third outcome `undef` which states that the rule or policy has no opinion on such request, a so called **policy gap** at which the policy is **“undefined”**. Semantically, a rule of form `grant if cond` is therefore really a shorthand for
```
grant if cond else undef
```
and a similar shorthand meaning applies to rules of form `deny if cond`.

The open and distributed nature of the Tangle will often generate  situations in which more than one rule may apply; an overall access-control policy may thus have evidence for grant as well as evidence for deny. **Such an apparent conflict should be made explicit in the language itself**, so that policy compositions can reflect and then appropriately act on it. This suggests a fourth value conflict as policy outcome, to denote such conflict as a result of policy evaluation.

It should be stressed that decisions `undef` and `conflict` are not enforceable as such by a PEP. But they are important to have and compute over for at least two reasons:
- these values can facilitate policy composition, e.g. a policy that returns `undef` may be **ignored** within a composition with another policy.
- policy analysis can discover requests for which policies may have gaps or conflicts, to aid the verification of the correctness of policies, their refinements, and compositions.

The conceptual grammar for our access-control language is depicted below. The syntactic category `dec` ranges over all four possible policy decisions `grant`, `deny`, `undef`, and `conflict`. A `term` either refers to a `constant` (the set of `constants` may subsume keywords such as `subject`, `action`, and `object`), an `entity` (a variable), the application of an `n`-ary operator `op` to `n` many terms (where `n > 1`), or a `term` indexed by an `attribute`. The choice of `n`-ary operators is in part domain-specific but in part also generic (e.g. operators for arithmetic). Note that `terms` are assumed to be well-typed given their intended meaning, which our implementation does reflect. The index operation allows us to write terms such as `vehicle.owner.daughter` where our type discipline will rule out conditions such as `vehicle.owner.daughter < localTime`.

```
dec   ::= grant | deny | undef | conflict
term  ::= constant | entity | op(term, ... , term) | term.attribute
cond  ::= (term == term) | (term < term) | (term ≤ term) | ...
           true | ¬cond | (cond && cond) | (cond || cond)
rule  ::= grant if cond | deny if cond
guard ::= true | pol eval dec | (guard && guard)
pol   ::= dec | rule | case { [guard : pol]⁺ [true : pol] }
```

The syntactic clause `cond` for conditions captures propositional logic with `true` denoting truth, `¬` negation, `&&` conjunction, and `||` disjunction. Its atomic expressions are obtained by relational operators applied to `terms`. The set of relational operators will contain generic ones such as those for equality and inequality (say over the integers) but also more domain-specific ones – e.g. a ternary one saying that an entity is the parent of two other entities. The category `rule` is defined as already discussed. A policy `pol` is either a constant policy `dec`, a rule `rule` or a `case`-policy.

The latter contains at least two cases, where a `case` is a pair of a `guard` and a policy `pol` and the last case has the catch-all guard `true`. Guards are essentially conjunctions of expressions of form `pol eval dec` which specify that policy `pol` evaluates to decision `dec`. A `case`-policy evaluates to the first policy `pol`ᵢ of a pair `guardᵢ : polᵢ` within that `case`-policy for which `guardᵢ` is `true`. It is helpful to think of the policies within guards as well as the `polᵢ` as sub-policies of the composition that is specified by a `case`-policy.

Further note that the grammar for terms is extensible and may contain clauses that plug in domain-specific aspects. In an advanced digital manufacturing plant, e.g., we may be interested in a ternary operation on terms denoting that two specific robots belong to the same team of workers.

For a policy `pol`, let us assume that the PIPs are able to obtain values for all attributes occurring in `pol`, so that the values of all terms of the policy can be computed. From the values for terms, we can then compute the truth values of all atomic conditions within policy `pol`. And from that we may follow the semantics of rules and of `case`-policies (the latter used for policy composition) to compute the outcome of this policy – which is either `grant`, `deny`, `undef` or `conflict`.

## Functional Completeness

Let us write:

```
4 = {grant, deny, undef, conflict}
```

for the set of possible policy decisions. Ideally, we would like to have a set of policy composition operators that can express all possible functions of type `4ⁿ → 4` for arities `n > 0` where composition operators for `n = 0` merely return a decision seen as a constant policy. This will guarantee that any policy behavior of this type can be written in the Policy Language.

Note that the clause `pol eval dec` together with conjunction allows us to express any one of the `4ⁿ` many rows in a “truth table” for a function `f` of type `4ⁿ → 4`. This means that syntactic clause guard can express any such truth table row. Thus, a case-policy can go through all the `4ⁿ` cases of that function `f` and return in each case the decision specified by `f` for that row as a constant policy.

This means that the Policy Language is **functionally complete** as a policy composition language: for any `n > 0` and any function `f : 4ⁿ → 4` we can define a bespoke **syntactic operator** over `n` policies that has the meaning of `f` and is expressive in the policy language demonstrated above. This is not just a theoretical result. It also gives us assurance that any policy composition needs that arise in use cases or specific application domains can be expressed as “macros” that compile into the above policy language, provided the meaning of such compositions is a function of the meanings of its argument policies.

Let us illustrate this result for the operation join. The intuition of `P join Q` is that it combines the results of policies `P` and `Q` so that the result is an **information join**. Namely, that this join returns

 - the result of one of these policies if the other one returns `undef`.
 - `conflict` if one of the policies returns `conflict`; or if one of them returns `grant` and the other one returns `deny`.
 - the result of policy `P` in all other cases (which will be the same result as that of `Q`).

We can express this specification of policy behavior in the Access Language as seen below. The encoding involves only 7 cases and not 4² = 16 cases as the above method for “truth tables” would construct.

```
case {
  [(P eval undef) : Q]
  [(Q eval undef) : P ]
  [(P eval conflict) : conflict]
  [(Q eval conflict) : conflict]
  [((P eval deny) && (Q eval grant)) : conflict]
  [((P eval grant) && (Q eval deny)) : conflict]
  [true : P ]
}
```

## Intermediate Language and Policy Verification

PEPs can only enforce policies that are free of gaps and conflicts; such policies are Boolean conditions – true for exactly those requests that will be granted. We now define two compilations of policies into such Boolean conditions which can be used to both verify that policies  written in the FROST language are  enforceable and for generating the to be enforced condi- tions, which implementations may represent in different form – e.g., as Boolean Circuits or Binary Decision Diagrams.


These compilations provide also the formal underpinnings for a whole range of policy analyses that have  important applications, such as a formal verification that a Boolean circuit does indeed faithfully  represent a particular access-control policy written in our policy language.

## Compiling Policies into Boolean Conditions

The four possible decisions `grant`, `deny`, `undef` and `conflict` can be represented in the 4-valued Belnap bi-lattice depicted in the Figure below. On this bi-lattice, we can define two subsets: `GoC = {grant, conflict}`  and `DoC = {deny, conflict}`. We can uniquely identify an element of that bi-lattice by saying whether or not  it is a member of `GoC`  and of `DoC`. For example, the element `undef`  is the unique one that is neither in `GoC` nor in `DoC`. And `grant` is the unique element that is in `GoC` but not in `DoC`.

![drawing](/docs/images/belnap.png)

This simple observation holds the key to representing a policy pol in a join normal form as
```
pol ≡ (grant if GoC(pol)) join (deny if DoC(pol))
```

where `GoC(pol)` is a Boolean (2-valued) expression specifying the exact conditions under which `pol` computes to `grant` or `conflict`, and where `DoC(pol)` is a Boolean (2-valued) expression specifying the exact conditions for when pol computes to deny or conflict. The correctness/exactness of the specifications `GoC(pol)` and `DoC(pol)` is what makes the representation of pol correct.

The expressions `GoC(pol)` and `DoC(pol)` are defined inductively over the grammar of policies for the Access Language, shown below. These definitions make use of an auxiliary predicate `T(guard)` that specifies the exact conditions for when a guard expression `guard` is `true`. We point out that the definition of `T(guard)` appeals to the predicates `GoC(pol)` and `DoC(pol)` – making these three predicates mutually recursive.

```
GoC(grant) ≡ true
GoC(conflict) ≡ true
GoC(grant if cond) ≡ cond

GoC(deny) ≡ false
GoC(undef) ≡ false
GoC(deny if cond) ≡ false

GoC(case { [g₁ : p₁] ... [gₙ₋₁ : pₙ₋₁] [true : pₙ] }) ≡
(T(g₁) && GoC(p₁)) || (¬T(g₁) && T(g₂) && GoC(p₂)) || ...
 ... || (¬T(g₁) && ... && ¬T(gₙ₋₁) && T(true) && GoC(pₙ))
```

```
DoC(deny) ≡ true
DoC(conflict) ≡ true
DoC(deny if cond) ≡ cond

DoC(grant) ≡ false
DoC(undef) ≡ false
DoC(grant if cond) ≡ false

DoC(case { [g₁ : p₁] ... [gₙ₋₁ : pₙ₋₁] [true : p n ] }) ≡
(T(g₁) && DoC(p₁)) || (¬T(g₁) && T(g₂) && DoC(p₂)) || ...
... || (¬T(g₁) && ... && ¬T(gₙ₋₁) && T(true) && DoC(pₙ))
```

The equations above show how compilation `GoC(pol)` generates a condition that is `true` if and only if `pol` returns `grant` or `conflict`. Similarly, `DoC(pol)` is `true` if and only if `pol` returns `deny` or `conflict`. Both depend on function `T(guard)` that specifies the condition for when guard evaluates to `true`, defined below:

```
T(true) ≡ true
T(g 1 && g 2 ) ≡ T(g 1 ) && T(g 2 )

                    | GoC(pol) && DoC(pol)  if dec equals conflict
                    |¬GoC(pol) && DoC(pol)  if dec equals deny
T(pol eval dec) ≡ { | GoC(pol) && ¬DoC(pol) if dec equals grant
                    |¬GoC(pol) && ¬DoC(pol) if dec equals undef
```

It is easy to show that expressions of form `T(guard)`, `GoC(pol)` and `DoC(pol)` are such that they are  generated by the grammar for syntactic category `cond`. It is in that sensethat we may think of cond as an intermediate language for policies – keeping in mind that the outputs of two such expressions may have to be combined in a 4-valued join as specified above.

## Policy Verification

With these predicates at hand, it is now easy to express important policy analyses as satisfiability problems over the logic and theories that interpret atomic conditions, and the attributes that they contain. For example, a policy `pol` from the Policy Language is gap free iff the predicate `¬GoC(pol) && ¬DoC(pol)` is unsatisfiable. Similarly, a policy `pol` from the Policy Language is conflict free iff `GoC(pol) && DoC(pol)` is unsatisfiable. In particular, if a policy `pol` is gap free and conflict free, then we may use `GoC(pol)` as an intermediate representation of this policy.

This is a Boolean “circuit” that evaluates to `true` if the policy grants an access request, and to `false` if the policy denies an access request. This technology can also be used to verify the integrity of a representation of a policy pol from the Policy Language.

### Verification Example

Suppose, e.g., that `φ` is a Boolean Circuit that is claimed to faithfully represent `pol` in that truth of the circuit means `grant` and falsity means `deny`. Anyone can then verify this claim by:

* generating the conditions `GoC(pol)` and `DoC(pol)` as specified before.
* using a formal verification tool to show that
    - `¬GoC(pol) && ¬DoC(pol)` is unsatisfiable, and so `pol` is indeed gap free.
    - `GoC(pol) && DoC(pol)` is unsatisfiable, and so pol is indeed conflict-free.
* verifying that `GoC(pol)` and `φ` are logically equivalent.

If any of the verification tasks in items 2 or 3 fail, this means that the integrity of `φ` has been disproved; otherwise, we have proof that `φ` indeed faithfully represents the policy pol.

A similar set of tasks can show that `φᴳᵒᶜ` faithfully represents `GoC(pol)` and `φᴰᵒᶜ` faithfully represents `DoC(pol)` for a policy pol that may neither be free of gaps nor free of conflicts. This would then verify the integrity of these two Boolean Circuits, which essentially capture the join normal form of that policy pol.
We may also verify other aspects of policy administration. In change management, e.g. we may need assurance that an updated policy pol 0 is not more permissive than some currently sed policy pol. For example, a proof that

```
T pol' eval grant && (T(pol eval undef) || T(pol eval deny))
```

is unsatisfiable would show that policy `pol'` can never grant whenever policy `pol` either denies or has a gap.

The verification tools one would use for these and other validation tasks may vary. Rewrite rules may capture equational theories of operators for transforming policies into equivalent ones and tools could perform such rewrite logic. This can also be helpful for simplifying policies in a manner that is meaningful to users.

A PDP may need to process a policy that contains gaps or conflicts. For example, a policy may be  submitted off-line through a cybersecurity protocol and so the PDP cannot make any assumptions about the semantic behavior of that policy. A PDP may therefore need to wrap this policy, at the top-level, into an idiom that makes the composed policy enforceable for a PEP. A policy wrapper that forces all conflicts and gaps of a policy pol to be denials would then be

```
case {
    [pol eval undef : deny]
    [pol eval conflict : deny]
    [true : pol]
}
```

This deny-by-default composition pattern can not only be expressed in our policy language but may also be hard-coded in a PDP. For example, let a PDP execute two Boolean circuits `φᴳᵒᶜ` and `φᴰᵒᶜ` , where these circuits capture the meaning of `GoC(pol)` and `DoC(pol)`, respectively. The PDP may then combine the outputs of these circuits as seen in the truth table below:

| `GoC(pol)` | `DoC(pol)` | output of PDP |
|---|---|---|
| true | true | deny  |
| true | false | grant |
| false | true | deny |
| false | false | deny |

Combining outputs of circuits `GoC(pol)` and `DoC(pol)` so that the combination honors all grants and denials of `pol` but overrides all gaps and conflicts of `pol` into denial.
