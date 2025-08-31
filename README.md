# LaTeX Command Remover

## Description

A tool to remove commands from LaTeX source files. Originally intended to apply changes written in LaTeX files using both `easyReview` and `changes` packages, but can also remove any command the user specifies while optionally preserving one argument.

## Usage

```
apply-tex-changes texfile.tex [command1:action1:preserved_index1 ...]
```

- The action could be d (delete) or p (preserve).
Preserved index is relevant to the "preserve" action, indicating the argument whose content will be preserved (counting starts from 0 so 0 is the first argument). Default value is zero.
- If no command is specified, the default is to remove the easyReview and changes LateX packages commands while applying the stated changes.
- The command applies changes in the same file and renames the original file to the following format: `original_name.timestamp.bak` so you can always undo changes by this command if needed. I recommend to keep the backup files until you're sure that everything is as expected.
- It is recommended to include this command in the User Commands section of TeXstudio (or the equivalent in other IDEs) for easy, GUI based usage of this tool. After installation, add the following user command to TeXstudio: `apply-tex-changes %.tex` and label it `Apply Changes`, then you can call this tool on the open document from Tools -> User -> Apply Changes.

**While this tool creates a backup automatically for the source file, it is recommended to have your own backup before running the tool. I am not responsible for possible data loss due to unknown bugs or incorrect usage (like unsupported encodings).**

### Example

To remove all bold text commands while preserving the content of the bold text (first argument):

```bash
apply-tex-changes texfile.tex textbf:p:0
```

To completely remove all instances of the `\tiny` command:

```bash
apply-tex-changes texfile.tex tiny:d
```

## Supported Character Encoding

This tool supports any encoding that is fully backward compatible with ASCII like UTF-8 (because it uses 8 bit chars). If the command produces a corrupted file, please restore the last backup (name format: `original_name.timestamp.bak`).

**WARNING:** When using a non supported encoding, the program will either corrupt the file, possibly work or do nothing. So for guaranteed reliability, ensure that your file is ASCII or UTF-8 encoded.

## Installation

### Linux

Make sure you have a working `gcc` compiler, install it using your package manager (ex: `sudo apt install gcc` for Ubuntu/Debian).

Clone the repository and execute the install script as root:

```
git clone --depth 1 https://github.com/a-h-ismail/latex-command-remover.git
cd latex-command-remover
chmod +x ./install.sh
sudo ./install.sh
```

Done!

## Uninstallation

### Linux

Run the removal script (the same as above but with `remove.sh` instead of `install.sh`)

## License

This project is licensed under the GNU GPL-3.0-or-later.