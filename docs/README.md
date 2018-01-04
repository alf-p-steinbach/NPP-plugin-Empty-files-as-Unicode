# Notepad++ plugin: Empty files as Unicode
Automatically change the encoding of empty documents to Unicode.

Let 𝘜 be UTF-8 with BOM. When a buffer is activated and has not already been checked:

<ul><li style="list-style-type: none;">if the document is empty and its encoding isn’t Unicode, then<br/>  
  its encoding is set to 𝘜.</li></ul>

Ideally the “when a buffer…” should have been “when file a is opened or a new document is
created”, but apparently Notepad++ does not inform a plugin of its creation of new
documents. Also, ideally the forced encoding should have been the one
currently selected as default in Notepad++, but apparently Notepad++ does not
make the dynamic configuration info available to a plugin.

Author’s mail address: [alf.p.steinbach+npp@gmail.com]
