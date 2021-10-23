# bitcoin-addrs

Utilities to generate bitcoin addresses from public key
======================

License
-------
[The MIT License (MIT)](https://opensource.org/licenses/MIT)


Dependencies
-------------

These dependencies are required:

    ----------------------------------------
    Library         |  Description
    ----------------|-----------------------
    gnutls          | sha256 / base64 
    libgmp          | arbitrary precision arithmetic
    ----------------------------------------

Optional dependencies:

    ----------------------------------------
    Library         |  Description
    ----------------|-----------------------
    libsecp256k1    | crypto: sign / verify
    ----------------------------------------


## Build

### Linux / Debian 

#### install dependencies

    $ sudo apt-get install build-essential libgmp-dev gnutls-dev
    
#### build
    $ mkdir -p workspace
    $ cd workspace
    $ git clone https://github.com/chehw/bitcoin-addrs.git
    $ cb bitcoin-addrs
    $ make

## run
	### print usuage
    $ bin/pubkey_to_addrs --help
    
    ### run
    $ bin/pubkey_to_addrs "(pubkey_hex)"

