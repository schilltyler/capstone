
---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Refresher: Cryptographic Hash
:::

- Imagine an oracle with infinite memory
- You present an input. If it's new, the oracle flips 256 fair coins for a random binary sequence (the hash). If it's seen the input before, it returns the stored output
- **Cryptographic hash functions**, if secure, should be indistinguishable from such an oracle

---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Cryptographic Hash Functions
:::

- Non-example: **CRC32** – easy to collide
- **SHA-256** → Safe
- **SHA1** → Unsafe
- **MD5** → Very unsafe (yet sometimes still used)
- **BLAKE Family**: :fire:
- **SHA3**: Probably fine, just less common
- Hash functions provide *probabilistically unique* fingerprints for files. Collisions are *extremely* unlikely in practice

---

<!-- slide template="[[Split Vertical]]" -->
::: title
#### Remark about Cryptographic Hash Functions
:::

::: left
<!-- element style="font-size: 24px"-->
- They take arbitrary-length input and produce a fixed-size digest
- Probability of collisions is exponentially low
- Yet there are infinitely many collisions in principle. How is that possible?
:::

::: right
![[0x03_slide_7_image.png|600]]
:::

---

<!-- slide template="[[Base Slide]]"-->
::: title
#### Key Properties 
:::
- Arbitrary inputs → fixed-length bit strings, bits look random (p=1/2)
- Changing a single bit → completely different hash
- The function is public, and brute force is possible for low-entropy inputs
- Example: Google the hex output 

<small>`5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8`</small>


---
<!-- slide template="[[Base Slide]]" -->
::: title
#### Recommended Hash Functions
:::

- *Collision* = H(x) = H(y) for x ≠ y  
- **MD5**: Fast but broken. Collisions possible. Use with caution. Still an industry standard for sharing IOCs. Why? because hasing all of your old indexed data might be too expensive  
- **SHA-256**: Stronger, widely used.  
- BLAKE family
---

<!-- slide template="[[Split Vertical]]" -->
::: title
#### Hashing
:::

::: left
- On Linux: `sha256sum`, `md5sum`, or Python’s `hashlib`
:::

::: right
```python3
hashlib.sha256(b"hash me :)").hexdigest()
```

:::
