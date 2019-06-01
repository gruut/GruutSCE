# Tethys Standard Contract Engine

> The name of this engine has been changed to Tethys Standard Contract ContractEngine (TSCE) according to the change of the project name.
> The original name was "Gruut Standard Contract Engine."

### 1. Basic Structure of Tethys Standard Contract 

```xml
<contract>
    <head>
        <cid>example_contract1::user0001::TESTNET::TESTWORLD</cid>
        <after>2019-01-01T00:00:00+09:00</after>
        <before>2020-12-31T23:59:59+09:00</before>
        <desc>Example of standard contract</desc>
        <friend>
            <cid>example_contract2::user0001::TESTNET::TESTWORLD</cid>
            <cid>example_contract3::user0001::TESTNET::TESTWORLD</cid>
        </friend>
    </head>
    <body></body>
    <fee></fee>
</contract>
```

### 2. Libraries

#### 2.1. Open Source License
- JSON for Modern C++ (Niels Lohmann)
- TinyXml2 (Lee Thomason)
- Date (Howard Hinnant et al.)

#### 2.2. Non-open Source License
- Recyclable Counter with Confinement (DaeHun Nyang et al.)

### 3. License
- (TBA)