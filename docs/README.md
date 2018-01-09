# Notepad++ plugin: Empty files as Unicode
Automatically change the encoding of empty documents to Unicode.

Without this plugin Notepad++ will assume that any existing empty file is encoded
as Windows ANSI. With this plugin a Unicode encoding is assumed, and used when you
add and save some text.

Technically, here’s how it works. Let 𝘜 be the encoding last saved as default in the
general Notepad++  options, or, if that encoding isn’t Unicode, then let 𝘜 be UTF-8
with BOM. When a buffer is activated and has not already been checked:

<ul><li style="list-style-type: none;">if the document is empty and its encoding isn’t Unicode, then<br/>  
  its encoding is set to 𝘜.</li></ul>

Author’s mail address: alf.p.steinbach+npp@gmail.com
