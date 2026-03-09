# Creator guide: signing an Agreement Document

This document describes how the creator of VAPI OSP can prepare and sign a commercial Agreement Document so that the licensee can verify the authenticity of the permission. The agreement must be a **document** sent from the creator's **official email** and, when applicable, signed with a **cryptographic signature** (e.g. PGP/GPG).

---

## 1. Why signing matters

- The licensee can verify that the permission was actually issued by the creator, not a third party.
- A signature guarantees that the document text has not been altered after sending.
- The license (section 3A) states: if the creator has published a signature verification method (e.g. PGP), the document must be signed.

Recommendation: create a PGP key, publish the public key in CONTACT.txt or in the repository, sign every agreement with that key, and send the document from the official email.

---

## 2. What the Agreement Document must contain

Per section 3A of the license, the document must include:

- **Licensee identification** — name or legal entity.
- **Scope of permitted use** — what exactly is allowed (product, domain, territory, etc.).
- **Effective date**.
- **Explicit statement** that the creator permits the specified commercial use of the Software (VAPI OSP).
- If needed — a reference to a separate contract or agreement number (if you maintain records).

The document may be formatted as an email body or as an attachment (e.g. PDF or plain text file). If signing is used, the final text/file is signed.

---

## 3. How to create your signature (PGP/GPG)

### 3.1. Installing GnuPG

- **Windows:** download Gpg4win from https://www.gpg4win.org/ or install via `winget install GnuPG.GnuPG`.
- **macOS:** `brew install gnupg`
- **Linux:** usually pre-installed; if not: `sudo apt install gnupg` (Debian/Ubuntu) or equivalent.

Verify: run `gpg --version` in a terminal.

### 3.2. Generating a key pair (creator's key)

1. Open a terminal and run:

   ```bash
   gpg --full-generate-key
   ```

2. Choose key type: **RSA and RSA** (default), press Enter.

3. Key size: **4096** bits (recommended for long-term use).

4. Expiration: set a period (e.g. 2 years) or **0** (no expiration) if preferred.

5. Enter your **name** and **email**. The email must match the official address from which you send agreements (the one listed in CONTACT.txt). Additional emails can be added later with `gpg --edit-key <KEY_ID>` then `adduid`.

6. Set a **passphrase** for the private key. It is required for every signing operation; store it securely.

GnuPG will create a pair: a **private key** (kept only by you) and a **public key** (can be published).

### 3.3. Finding your key ID and fingerprint

Run:

```bash
gpg --list-secret-keys --keyid-format=long
```

Example output:

```
sec   rsa4096/ABCD1234EF567890 2026-01-15 [SC]
      XXXXXXXXXXXXXXXX...
uid                 [ultimate] Your Name <your@official-email.com>
```

- **KEY_ID** — the part after `rsa4096/`, e.g. `ABCD1234EF567890`.
- Fingerprint: `gpg --fingerprint YOUR_KEY_ID` — a long string that can be listed in CONTACT.txt for reliable identification.

### 3.4. Exporting and publishing the public key

Export in ASCII format (for embedding in a file or on a website):

```bash
gpg --armor --export YOUR_KEY_ID
```

Copy the block from `-----BEGIN PGP PUBLIC KEY BLOCK-----` to `-----END PGP PUBLIC KEY BLOCK-----` and:

- add it to CONTACT.txt under a "PGP public key" section, or
- save it in the repository, e.g. `licenses/CREATOR_PGP_PUBLIC_KEY.asc`, and reference the path in CONTACT.txt.

Optionally, upload the key to a key server so licensees can retrieve it by email or KEY_ID:

```bash
gpg --send-keys YOUR_KEY_ID
```

(By default the key is sent to keys.openpgp.org; a different server can be specified if needed.)

### 3.5. Signing an Agreement Document

**Option A: sign a text file (e.g. agreement.txt)**

1. Write the agreement text (with licensee data, scope of use, date, and explicit permission).
2. Save it to a file, e.g. `agreement.txt`.
3. Create the signature:

   ```bash
   gpg --sign --armor -o agreement.txt.asc agreement.txt
   ```

   You will be prompted for the private key passphrase. The result is `agreement.txt.asc` — the signed document in text form. Send both files to the licensee: `agreement.txt` and `agreement.txt.asc`, or a single combined file (see below).

**Option B: single-file signature (clearsigned)**

```bash
gpg --clearsign agreement.txt
```

This produces `agreement.txt.asc` containing both the original text and the signature. Convenient for sending as a single attachment.

**Option C: sign a PDF or other binary file**

```bash
gpg --sign --armor -o document.pdf.asc document.pdf
```

Send the licensee `document.pdf` and `document.pdf.asc`. Verification: `gpg --verify document.pdf.asc document.pdf`.

### 3.6. Sending from the official email

- Send the signed document (and the original file if applicable) **from the same email** listed in CONTACT.txt as the creator's official channel.
- In the email body you may briefly note: "Attached is a signed agreement; verify the signature with: gpg --verify file.asc [file]".

The licensee can then verify the signature against your public key and confirm that the document was sent by you and has not been tampered with.

---

## 4. How the licensee verifies the signature

1. Download the creator's public key from CONTACT.txt or from the repository.
2. Import the key: `gpg --import CREATOR_PGP_PUBLIC_KEY.asc`
3. To verify the signed file:
   - if the signature is in a separate file: `gpg --verify agreement.txt.asc agreement.txt`
   - if clearsigned: `gpg --verify agreement.txt.asc`
4. If GnuPG reports "Good signature" and shows the creator's key — the signature is valid.

---

## 5. Quick checklist for the creator

1. Install GnuPG and generate a key with the official email.
2. Publish the public key in CONTACT.txt (or in `licenses/CREATOR_PGP_PUBLIC_KEY.asc`) and optionally on a key server.
3. In every agreement include: licensee, scope of use, date, explicit permission.
4. Sign the document with the private key: `gpg --sign` or `gpg --clearsign`.
5. Send the signed document from the official email listed in CONTACT.txt.

This ensures compliance with the license requirements (section 3A): an agreement is a document sent from your email and, when applicable, verifiable by your signature.
