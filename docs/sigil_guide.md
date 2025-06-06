# Sigil

This is a brief introduction to Sigil, which is a small language for Redstone simulation.

## Basics

Sigil programs consist of a series of module declarations and system tasks.

### Module declarations

A module declaration is of the form
```
module module_name(arguments) -> outputs {
    [stuff]
}
```

Where `[stuff]` refers to a sequences of component instantiations, module instantiations,
and assignments.

### Component instantiations

Component instantiations look like one of the following.

```
dust [identifier];
torch [identifier];
repeater<[ticks]> [identifier];
subtract_comparator [identifier];
compare_comparator [identifier];
clock<[period], [pulse duration]> identifier;
```

`ticks`, `period`, and `pulse duration` must all be integers.
`ticks` must be between 1 and 4 inclusive.
`period` must be greater than its corresponding `pulse duration`.

These can be thought of as representing a given redstone component in a repeated module in a larger system.

### Module instantiations

Module instantiations look as follows:

```
[module_name] [identifier](arguments);
```

where the arguments correspond to the arguments in a previous module declaration.

These can be thought of as representing the replication of a smaller module inside a larger one.

### Assignments

Assignments look as follows:

```
[component identifier] = [argument];
[component identifier] = [component identifier]
[component identifier] = [module identifier].[output identifier];
[output identifier] = [argument];
[output identifier] = [component identifier];
[output identifier] = [module identifier].[output identifier];
```

These represent connecting one redstone component or module to another in sequence
(this does mean that it doesn't model "back-flow" in redstone circuits;
everything is assumed unidirectional).

Because in Minecraft, torches and dusts effectively perform a logical union on their inputs,
an assignment effectively adds to the union.

A comparator must have at most three inputs, and they are added in the following order: left, right, back.

### System tasks

Sigil currently supports exactly two system tasks.
```
$expect([timestamp], [module type]([integer arguments]).[output name], [expected value]);
$print([timestamp], [module type]([integer arguments]).[output name], 0);
```

These correspond to, on the level of Sigil,
* Checking that the named module with the given signals inputted to it will, at the given timestamp, return the desired value in the desired output field
* Printing what the named module with the given signals inputted to it will, at the given timestamp, return in the desired output field (the additional 0 at the end of `$print` is due to the limitations of my frontend)


## Example 0: a logical AND gate

A logical AND gate can be represented as demonstrated in `sigil-src/and.sigil`.

As you can see, it defines three torch components, `torch_l`, `torch_r`, and `torch_c`,
then chains them to return the output.

This logically makes sense because a torch is basically
```math
\neg \biggr(\bigcup_{i \in \text{inputs}} i \biggr)
```

So the defined circuit is the following:

```math
\neg \biggr( \neg\text{left} \bigcup \neg \text{right} \biggr)
```
By DeMorgan's law, this is equivalent to a logical AND.

## Example 1: a logical XOR gate

A logical XOR gate cna be represented as demonstrated in `sigil-src/xor.sigil`.

This is more complex than the AND gate but is still simple enough.

As we've established before, a `torch` is a negation of a union.
A `dust` is a simple union.

So, skipping about halfway through the code,
since we have `nand_inputs`, `torch_bottom_l`, and `torch_bottom_r` passed into torch `center`,
we have

```math
\text{center} = \neg \biggr( \biggr[ \neg \text{left} \bigcup \neg \text{right} \biggr] \bigcup \neg \text{left} \bigcup \neg \text{right} \biggr)

= \neg \biggr[ \neg \text{left} \bigcup \neg \text{right} \biggr] \bigcap \text{left} \bigcap \text{right}

= \text{left} \bigcap \text{right}
```

Given this, we have the `torch_final_l` and  `torch_final_r` components,
which are as follows
```math
\text{torch_final_l} = \neg \biggr( \biggr[ \text{left} \bigcap \text{right} \biggr] \bigcup \neg \text{left} \biggr)
= \neg \biggr[ \text{left} \bigcap \text{right} \biggr] \bigcap \text{left}
= \biggr[ \neg \text{left} \bigcup \neg \text{right} \biggr] \bigcap \text{left}
= \neg \text{right} \bigcap \text{left}
```

```math
\text{torch_final_r} = \neg \biggr( \biggr[ \text{left} \bigcap \text{right} \biggr] \bigcup \neg \text{right} \biggr)
= \neg \biggr[ \text{left} \bigcap \text{right} \biggr] \bigcap \text{right}
= \biggr[ \neg \text{left} \bigcup \neg \text{right} \biggr] \bigcap \text{right}
= \neg \text{left} \bigcap \text{right}
```

These two components are combined by `result`,
meaning
```math
\text{result} = \biggr[ \text{left} \bigcap \neg \text{right} \biggr] \bigcup \biggr[ \neg \text{left} \bigcap \text{right} \biggr]
```

This is basically the definition of a logical XOR, so we win.

## Example 2: a half-adder

Now that we have a XOR and an AND gate,
we can build a half-adder, as described in `sigil-src/half-adder.sigil`.

Since this is purely binary arithmetic on two bits,
we know that the least significant bit is the XOR of the inputs, and that the carry is the AND of the inputs.

## Example 3: a full-adder

Now that we have a half-adder, you can try it for yourself and combine half-adders to form a full adder.
With two adders and a union (which we get by a dust) we can make a full-adder that accepts a carry bit!
Check out `sigil-src/full-adder.sigil`.

## Example 4: a four-bit adder

Now that we have full-adders, we can chain them together to actually get binary addition.
This works for four bits but we could apply it to more if we wanted to!
Take a look at `sigil-src/4-bit-adder.sigil`.

