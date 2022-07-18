;; Show init.el file on startup(setq initial-buffer-choice "~/.emacs.d/init.el");; Trying out different themes:;; (load-theme 'wombat);; (load-theme 'solarized-dark);; (load-theme 'tango-dark);; (load-theme 'melancholy); Stop Emacs from losing undo information by; setting very high limits for undo buffers(setq undo-limit 20000000)(setq undo-strong-limit 40000000); Test popup stuff;; (setq ns-pop-up-frames nil);; (setq pop-up-frames t);; Set font and fontsize(set-face-attribute 'default nil :font "Source Code Pro" :height 105)(setq inhibit-startup-message t)           ; Don't show start screen(setq visible-bell t)                      ; Set up the visible bell(setq delete-by-moving-to-trash t)         ; Don't ever delete files permanently(setq mouse-wheel-progressive-speed nil)   ; Don't increase scrolling amount (setq window-min-height 2)(setq window-min-width 4)(setq windsize-cols 1)(setq windsize-rows 1)(setq doc-view-continuous t)(setq compilation-scroll-output t)         ; Follow compilation output(setq use-package-verbose t)               ; Show load time of packages;;(setq special-display-regexps (.*))(setq-default word-wrap t)                 ; Wrap line on word boundaries, not on characters(global-visual-line-mode 1)                ; Make commands act on visual lines instead of logical lines(setq-default indent-tabs-mode nil)        ; Required to make autoindent work with tab size;; (setq-default tab-width 4)                 ; Set number of spaces per tab;; (setq indent-line-function 'insert-tab)    ; Set autoindent to tab(setq-default c-basic-offset 4);; Set path for diff capability (installed with git)(setq ediff-diff-program "C:/Program Files/Git/usr/bin/diff.exe")(setq ediff-split-window-function 'split-window-horizontally      ediff-window-setup-function 'ediff-setup-windows-plain);; Change color of diff highlighting(custom-set-faces ;; custom-set-faces was added by Custom. ;; If you edit it by hand, you could mess it up, so be careful. ;; Your init file should contain only one such instance. ;; If there is more than one, they won't work right. '(ediff-current-diff-A ((t (:background "DodgerBlue4")))) '(ediff-current-diff-B ((t (:background "DodgerBlue4")))));; (load-file "~/.emacs.d/gnus_config.el");; (add-hook 'after-init-hook 'init-gnus)(add-hook 'emacs-startup-hook 'init-treemacs);; (add-hook 'emacs-startup-hook 'init-shell);; (add-hook 'prog-mode-hook 'highlight-numbers-mode)(scroll-bar-mode -1)               ; Disable visible scrollbar(tool-bar-mode -1)                 ; Disable the toolbar(tooltip-mode -1)                  ; Disable tooltips(set-fringe-mode 5)                ; Give some breathing room(menu-bar-mode -1)                 ; Disable the menu bar(global-display-line-numbers-mode) ; Display line numbers(show-paren-mode 1)                ; Highlight matching parenthesis(toggle-frame-maximized)           ; Toggles full-screen on startup(display-time)                     ; Displays time in mode-line(global-hl-line-mode 1)            ; Toggle highlighting for current line(delete-selection-mode 1)          ; Delete selected text if you start typing after selection;; Define custom keybinds;; (global-set-key (kbd "<escape>")          'keyboard-escape-quit)(global-set-key (kbd "C-x C-o")             'delete-window);; (global-set-key (kbd "C-d")                 'forward-char);; (global-set-key (kbd "C-a")                 'backward-char)(global-set-key (kbd "C-e")                 'forward-word)(global-set-key (kbd "C-q")                 'backward-word)(global-set-key (kbd "M-e")                 'move-end-of-line)(global-set-key (kbd "M-q")                 'move-beginning-of-line)(global-set-key (kbd "M-n")                 'forward-sentence)(global-set-key (kbd "M-p")                 'backward-sentence)(global-set-key (kbd "C-r")                 'comment-or-uncomment-region-or-line)(global-set-key (kbd "C-S-d")               "\C-a\C- \C-n\M-w\C-y\C-p")(global-set-key (kbd "C-S-w")               "\C-a\C- \C-e\M-w")(global-set-key (kbd "C-v")                 'yank)(global-set-key (kbd "C-z")                 'undo)(global-set-key (kbd "C-f")                 'isearch-forward)(define-key isearch-mode-map "\C-f"         'isearch-repeat-forward) ; makes it behave like the standard bind C-s(global-set-key (kbd "C-c C-c")             'kill-ring-save)(global-set-key (kbd "C-s")                 'save-buffer)(global-set-key (kbd "C-x C-k")             'kill-current-buffer)(global-set-key (kbd "<tab>")               'tab-region)(global-set-key (kbd "<backtab>")           'untab-region)(global-set-key (kbd "C-x C-b")             'switch-to-buffer)(global-set-key (kbd "C-S-f")               'elgrep)(global-set-key (kbd "C-o")                 'other-window)(global-set-key (kbd "M-<")                 'windsize-up)(global-set-key (kbd "M->")                 'windsize-down)(global-set-key (kbd "C-<")                 'windsize-left)(global-set-key (kbd "C->")                 'windsize-right)(global-set-key (kbd "<f5>")                'my-compile-command)(global-set-key (kbd "<f6>")                'my-run-command);; (global-set-key (kbd "<f9>")                'fetch-mail)(global-set-key (kbd "<f8>")                'xref-find-definitions-other-window)(global-set-key (kbd "C-ö")                 "{")(global-set-key (kbd "C-ä")                 "}")(global-set-key (kbd "C-ü")                 "[")(global-set-key (kbd "C-+")                 "]")(global-set-key (kbd "<f12>")               'refresh-buffer)(global-set-key (kbd "C-c <C-right>")       'next-error)(global-set-key (kbd "C-c <C-left>")        'previous-error)(global-set-key (kbd "<f9>")        'ff-find-other-file);; Enables to search immediately for highlighted expression when starting isearch command(defadvice isearch-mode (around isearch-mode-default-string (forward &optional regexp op-fun recursive-edit word-p) activate)  (if (and transient-mark-mode mark-active (not (eq (mark) (point))))      (progn        (isearch-update-ring (buffer-substring-no-properties (mark) (point)))        (deactivate-mark)        ad-do-it        (if (not forward)            (isearch-repeat-backward)          (goto-char (mark))          (isearch-repeat-forward)))    ad-do-it));; Make ctrl-backspace killword instead of the hungry delete that overwrites it in python mode(add-hook 'python-mode-hook  (lambda ()     (define-key python-mode-map (kbd "C-c C-c")       'kill-ring-save)     (define-key python-mode-map (kbd "<C-backspace>") 'backward-kill-word)     (define-key python-mode-map (kbd "C-S-d")         "\C-a\C- \C-n\M-w\C-y\C-p")    )  )(add-hook 'c++-mode-hook  (lambda ()      (define-key c++-mode-map (kbd "C-c C-c")    'kill-ring-save)    )  )(add-hook 'tex-mode-hook  (lambda ()    (define-key tex-mode-map (kbd "<f9>")         'TeX-next-error)    )  )(add-hook 'compilation-mode-hook  (lambda ()     (define-key compilation-mode-map (kbd "C-o") 'other-window)    )  )(font-lock-add-keywords 'python-mode  `((,(concat       "\\<[_a-zA-Z][_a-zA-Z0-9]*\\>"       ; Object identifier       "\\s *"                              ; Optional white space       "\\(?:\\.\\|->\\)"                   ; Member access       "\\s *"                              ; Optional white space       "\\<\\([_a-zA-Z][_a-zA-Z0-9]*\\)\\>" ; Member identifier       "\\s *"                              ; Optional white space       "(")                                 ; Paren for method invocation       1 'font-lock-function-name-face)))(font-lock-add-keywords 'c++-mode  `((,(concat       "\\<[_a-zA-Z][_a-zA-Z0-9]*\\>"       ; Object identifier       "\\s *"                              ; Optional white space       "\\(?:\\.\\|->\\)"                   ; Member access       "\\s *"                              ; Optional white space       "\\<\\([_a-zA-Z][_a-zA-Z0-9]*\\)\\>" ; Member identifier       "\\s *"                              ; Optional white space       "(")                                 ; Paren for method invocation       1 'font-lock-function-name-face)));; Function definitions(defun connect-remote-ollie ()  (interactive)  (dired "/ssh:tamir@ollie:/"))(defun init-treemacs nil  (interactive)  (treemacs)  (other-window -1)  )(defun init-shell nil  (interactive)  (shell-command "D:\\programming\\shell.bat")  )(defun refresh-buffer nil  (interactive)  (revert-buffer t t t)  (end-of-buffer)  )(defun search-in-current-dir nil  (interactive)  (elgrep)  ;; "<RETURN>"  ;; "<RETURN>"     );; (defun fetch-mail nil;     (interactive);;     ;; (split-window-below);;     (gnus-fetch-group "mail.misc");;     ;; (gnus);;  )(defun my-split-window-right (&optional arg)  "Split the current window 70/30 rather than 50/50.  A single-digit prefix argument gives the top window arg*10%."  (interactive "P")  (let ((proportion (* (or arg 3) 0.1)))    (split-window-right (round (* proportion (window-width))))))(defun my-split-window-below (&optional arg)  "Split the current window 70/30 rather than 50/50.  A single-digit prefix argument gives the top window arg*10%."  (interactive "P")  (let ((proportion (* (or arg 9) 0.1)))    (split-window-below (round (* proportion (window-height))))))(defun my-run-command nil  (interactive)    (if (string-equal mode-name "C++//l")      (progn      ;; (shell-command "D:\\programming\\run.bat")      (shell-command "run.bat")	)()	 )    )(defun my-compile-command nil    (interactive)  (when (get-buffer "*compilation*")  (shrink-window 10))  (setq split-width-threshold nil)  (if (string-equal mode-name "Python")      (progn        (setq compile-command (concat "python36 " buffer-file-name))	(compile compile-command)	)()     )  (if (string-equal mode-name "Tcl")      (progn        (setq compile-command (concat "tclsh " buffer-file-name))	(compile compile-command)	)()	 )  (if (string-equal mode-name "C++//l")      (progn        ;; (setq compile-command "D:\\programming\\shell.bat & D:\\programming\\build.bat")        (setq compile-command "D:\\programming\\gui\\src\\shell.bat & D:\\programming\\gui\\src\\build.bat")	(compile compile-command)	)()	 )  (setq split-width-threshold 160)   ;; (switch-to-buffer (get-buffer "*compilation*"))  ;; (end-of-buffer)  ;; (previous-buffer)    (enlarge-window 10)  )(defun indent-region-custom(numSpaces)  (progn    ;; default to start and end of current line    (setq regionStart (line-beginning-position))    (setq regionEnd (line-end-position))    ;; if there's a selection, use that instead of the current line    (when (use-region-p)      (setq regionStart (region-beginning))      (setq regionEnd (region-end))      )    (save-excursion                            ; restore the position afterwards      (goto-char regionStart)                  ; go to the start of region      (setq start (line-beginning-position))   ; save the start of the line      (goto-char regionEnd)                    ; go to the end of region      (setq end (line-end-position))           ; save the end of the line      (indent-rigidly start end numSpaces)     ; indent between start and end      (setq deactivate-mark nil)               ; restore the selected region      )    )  )(defun untab-region (N)  (interactive "p")  (indent-region-custom -4)  )(defun tab-region (N)  (interactive "p")  (if (active-minibuffer-window)      (minibuffer-complete)          ; tab is pressed in minibuffer window -> do completion    (if (use-region-p)               ; tab is pressed in any other buffer -> execute with space insertion        (indent-region-custom 4)     ; region was selected, call indent-region-custom      (insert "    ")                ; else insert four spaces as expected      )    )  )(defun comment-or-uncomment-region-or-line ()    "Comments or uncomments the region or the current line if there's no active region."    (interactive)    (let (beg end)        (if (region-active-p)            (setq beg (region-beginning) end (region-end))            (setq beg (line-beginning-position) end (line-end-position)))        (comment-or-uncomment-region beg end)))(defun my-c++-mode-hook ()  (setq c-basic-offset 4)  (c-set-offset 'substatement-open 0))(add-hook 'c++-mode-hook 'my-c++-mode-hook);; (defun init-gnus nil;;    (interactive);;    ;; (gnus-fetch-group "mail.misc");;    (gnus);; );; Initialize package sources(require 'package)(setq package-archives '(("melpa" . "https://melpa.org/packages/")                         ("org"   . "https://orgmode.org/elpa/")                         ("elpa"  . "https://elpa.gnu.org/packages/")))(package-initialize)(unless package-archive-contents (package-refresh-contents));; Initialize use-package on non-Linux platforms(unless (package-installed-p 'use-package)   (package-install 'use-package))(require 'use-package)(setq use-package-always-ensure t)(use-package modern-cpp-font-lock  :ensure t)(modern-c++-font-lock-global-mode t);; (use-package flycheck);; (global-flycheck-mode); Loads doom-theme and applies it to treemacs(use-package doom-themes  :ensure t  ;; :defer t  :config  ;; NOTE:  ;; Themes:  ;; city-lights  ;; gruvbox  ;; horizon  ;; material  ;; moonlight  ;; palenight  :init (load-theme 'doom-gruvbox t)  ;; Global settings (defaults)  (setq doom-themes-enable-bold t    ; if nil, bold is universally disabled        doom-themes-enable-italic t) ; if nil, italics is universally disabled  ;; (load-theme 'doom-one t)  ;; Enable flashing mode-line on errors  ;; (doom-themes-visual-bell-config)  ;; Enable custom neotree theme (all-the-icons must be installed!)  ;; (doom-themes-neotree-config)  ;; or for treemacs users  (setq doom-themes-treemacs-theme "doom-colors") ; use "doom-colors" for less minimal icon theme  (doom-themes-treemacs-config)  ;; Corrects (and improves) org-mode's native fontification.  ;; (doom-themes-org-config)  );; Lines adapted from Casey's init.el:;; (set-face-attribute 'font-lock-builtin-face nil :foreground "indian red")(set-face-attribute 'font-lock-comment-face nil :foreground "gray52")(set-face-attribute 'font-lock-constant-face nil :foreground "CadetBlue3")(set-face-attribute 'font-lock-doc-face nil :foreground "gray52")(set-face-attribute 'font-lock-function-name-face nil :foreground "indian red")(set-face-attribute 'font-lock-keyword-face nil :foreground "DarkGoldenRod3")(set-face-attribute 'font-lock-string-face nil :foreground "#869b59")(set-face-attribute 'font-lock-type-face nil :foreground "#e3bf28")(set-face-attribute 'font-lock-variable-name-face nil :foreground "burlywood3")(set-foreground-color "burlywood3")(set-background-color "#161616");; (set-background-color "#222222");; (set-cursor-color "#40FF40");; Modifies the modeline with some symbols and git status stuff(use-package doom-modeline  ;; :after (doom-themes)  :ensure t  :init (doom-modeline-mode 1)  :custom    ((doom-modeline-height 10))    )(set-face-attribute 'doom-modeline-buffer-modified nil :foreground "red" :bold t)(use-package highlight-numbers)(add-hook 'prog-mode-hook 'highlight-numbers-mode)(set-face-attribute 'highlight-numbers-number nil :foreground "#869b59" :bold nil);; Enables window with command log that helps with learning emacs(use-package command-log-mode);; Improves the completion and UI of finding anything from commands to files and buffers etc(use-package vertico  :ensure t  :custom  (vertico-cycle t)  :init  (vertico-mode));; Saves the history of commands such that they can be easily repeated and looked up in the vertico list(use-package savehist  :init  (savehist-mode));; Shows additional information about buffers and files (like size and status) in the vertico list(use-package marginalia  :after vertico  :ensure t  :custom  (marginalia-annotators '(marginalia-annotators-heavy marginalia-annotators-light nil))  :init  (marginalia-mode));; Whether display the gnus notifications.;; (setq doom-modeline-gnus t);; Wheter gnus should automatically be updated and how often (set to 0 or smaller than 0 to disable);; (setq doom-modeline-gnus-timer 1);; Exclude groups from notification display;; (setq doom-modeline-gnus-excluded-groups '("nndraft:drafts"));; Set Python interpreter(setq python-shell-interpreter "python36");; Set Tcl interpreter(setq tcl-application "tclsh");; Setup git interface;; (use-package magit;;   :custom;;   (magit-display-buffer-function #'magit-display-buffer-same-window-except-diff-v1));; NOTE: Make sure to configure a GitHub token before using this package!;; - https://magit.vc/manual/forge/Token-Creation.html#Token-Creation;; - https://magit.vc/manual/ghub/Getting-Started.html#Getting-Started;; (use-package forge)  ;; Help for keybindings (good for learning)(use-package which-key  :init (which-key-mode)  :diminish which-key-mode  :config  (setq which-key-idle-delay 1));; Project management library(use-package projectile  :diminish projectile-mode  :config (projectile-mode)  :custom ((projectile-completion-system 'ivy))  :bind-keymap  ("C-c p" . projectile-command-map)  :init  ;; NOTE: Set this to the folder where you keep your Git repos!  (when (file-directory-p "D:/")    (setq projectile-project-search-path '("D:/")))  (setq projectile-switch-project-action #'projectile-dired));; Helps with auto-completion(use-package company :ensure t :config (setq company-idle-delay 0       company-minimum-prefix-length 2       company-show-numbers t       company-tooltip-limit 10       company-tooltip-align-annotations t       ;; invert the navigation direction if the the completion popup-isearch-match       ;; is displayed on top (happens near the bottom of windows)       company-tooltip-flip-when-above t) (global-company-mode t) );; (use-package company-clang)(use-package python-mode);; Anaconda setup (use-package anaconda-mode  :ensure t  :config  ;; :bind  ;; (:map global-map  ;;       ("M-."       . 'anaconda-mode-find-definitions-other-window)  (add-hook 'python-mode-hook 'anaconda-mode)  (add-hook 'python-mode-hook 'anaconda-eldoc-mode)  );; Virtual environment, helps with anaconda-mode completion(use-package pyvenv  :after anaconda-mode  :config  (pyvenv-mode 1));; Use company for anaconda-completions(use-package company-anaconda  :ensure t  :init (require 'rx)  :after (company)  :config  (add-to-list 'company-backends 'company-anaconda)  )(use-package company-quickhelp  ;; Quickhelp may incorrectly place tooltip towards end of buffer  ;; See: https://github.com/expez/company-quickhelp/issues/72  :ensure t  :config  (company-quickhelp-mode)  );; Setup sidebar that shows files in directory(use-package treemacs  :ensure t  :defer t  :init  (with-eval-after-load 'winum    (define-key winum-keymap (kbd "C-o") #'treemacs-select-window))  :config  (progn    (setq treemacs-deferred-git-apply-delay        0.5              treemacs-directory-name-transformer      #'identity          treemacs-display-in-side-window          t          treemacs-eldoc-display                   t          treemacs-file-event-delay                5000          treemacs-file-extension-regex            treemacs-last-period-regex-value          treemacs-file-follow-delay               0.2          treemacs-file-name-transformer           #'identity          treemacs-follow-after-init               t          treemacs-expand-after-init               t          treemacs-git-command-pipe                ""          treemacs-goto-tag-strategy               'refetch-index          treemacs-indentation                     2          treemacs-indentation-string              " "          treemacs-is-never-other-window           nil          treemacs-max-git-entries                 5000          treemacs-missing-project-action          'ask          treemacs-move-forward-on-expand          nil          treemacs-no-png-images                   nil          treemacs-no-delete-other-windows         t          treemacs-project-follow-cleanup          nil          treemacs-persist-file                    (expand-file-name ".cache/treemacs-persist" user-emacs-directory)          treemacs-position                        'left          treemacs-read-string-input               'from-child-frame          treemacs-recenter-distance               0.1          treemacs-recenter-after-file-follow      nil          treemacs-recenter-after-tag-follow       nil          treemacs-recenter-after-project-jump     'always          treemacs-recenter-after-project-expand   'on-distance          treemacs-litter-directories              '("/node_modules" "/.venv" "/.cask")          treemacs-show-cursor                     nil          treemacs-show-hidden-files               t          treemacs-silent-filewatch                nil          treemacs-silent-refresh                  nil          treemacs-sorting                         'alphabetic-asc          treemacs-select-when-already-in-treemacs 'move-back          treemacs-space-between-root-nodes        t          treemacs-tag-follow-cleanup              t          treemacs-tag-follow-delay                1.5          treemacs-text-scale                      nil          treemacs-user-mode-line-format           nil          treemacs-user-header-line-format         nil          treemacs-wide-toggle-width               70          treemacs-width                           25          treemacs-width-increment                 1          treemacs-width-is-initially-locked       t          treemacs-workspace-switch-cleanup        nil)    ;; The default width and height of the icons is 22 pixels. If you are    ;; using a Hi-DPI display, uncomment this to double the icon size.    ;;(treemacs-resize-icons 44)    (treemacs-follow-mode t)    (treemacs-filewatch-mode t)    (treemacs-fringe-indicator-mode 'always)    (pcase (cons (not (null (executable-find "git")))                 (not (null treemacs-python-executable)))      (`(t . t)       (treemacs-git-mode 'deferred))      (`(t . _)       (treemacs-git-mode 'simple)))    )    ;; (treemacs-hide-gitignored-files-mode nil))  :bind  (:map global-map        ("M-0"       . treemacs-select-window)        ("C-x t 1"   . treemacs-delete-other-windows)        ("<f3>"      . treemacs)        ("C-x t B"   . treemacs-bookmark)        ("C-x t C-t" . treemacs-find-file)        ("C-x t M-t" . treemacs-find-tag)));; Set correct python path(setq treemacs-python-executable "C:\\Program Files\\Python36\\python36.exe");; (use-package treemacs-evil;;   :after (treemacs evil);;   :ensure t)(use-package treemacs-projectile  :after (treemacs projectile)  :ensure t)(use-package treemacs-icons-dired  :hook (dired-mode . treemacs-icons-dired-enable-once)  :ensure t)(use-package treemacs-magit  :after (treemacs magit)  :ensure t);; For searching strings in multiple files(use-package elgrep);; Add different facecolors for useful tags in comments with search functionality(use-package hl-todo)(setq hl-todo-keyword-faces      '(	("TODO"   . "#FF0000")        ("FIXME"  . "#b22222")        ("DEBUG"  . "#A020F0")        ("HACK"   . "#daa520")        ("NOTE"   . "#ffdab9")       )      )(global-hl-todo-mode 1)            ; Enable tag highlighting in comments globally;; Indents wrapped lines according to the original logical line(use-package adaptive-wrap)(add-hook 'latex-mode-hook 'adaptive-wrap-prefix-mode);; (use-package reftex);; (use-package auto-complete-auctex);; (use-package tex-mode  ;; :ensure auctex);; (use-package company-auctex);; (use-package reftex);; (use-package texfrag);; (texfrag-global-mode);; (add-hook 'doc-view-mode-hook 'auto-revert-mode);; (add-hook 'LaTeX-mode-hook 'turn-on-reftex);; (setq-default TeX-master nil);; (setq-default TeX-auto-save t)(use-package windsize);; (windsize-default-keybindings)(use-package  multiple-cursors)(global-set-key (kbd "C-c C-l") 'mc/edit-lines)(use-package  msvc);; (global-set-key (kbd "C-c C-l") 'mc/edit-lines)(use-package markdown-mode  :ensure t  :mode ("README\\.md\\'" . gfm-mode)  :init (setq markdown-command "multimarkdown"));; (autoload 'markdown-mode "markdown-mode";;    "Major mode for editing Markdown files" t);; (add-to-list 'auto-mode-alist;;              '("\\.\\(?:md\\|markdown\\|mkd\\|mdown\\|mkdn\\|mdwn\\)\\'" . markdown-mode));; (autoload 'gfm-mode "markdown-mode";;    "Major mode for editing GitHub Flavored Markdown files" t);; (add-to-list 'auto-mode-alist '("README\\.md\\'" . gfm-mode))(setq auto-mode-alist      (append auto-mode-alist              '(("\\.log\\'" . auto-revert-mode)                ("\\messageLog.txt\\'" . auto-revert-mode))));; (add-to-list 'auto-mode-alist '("\\.log\\'" . auto-revert-mode))(defun etc-log-tail-handler ()  (end-of-buffer)  ;; (make-variable-buffer-local 'auto-revert-interval)  (setq auto-revert-interval 5)  ;; (setq auto-revert-use-notify nil)  ;; (auto-revert-set-timer)  ;; (make-variable-buffer-local 'auto-revert-verbose)  ;; (setq auto-revert-verbose nil)  (read-only-mode t))  ;; (font-lock-mode 0)  ;; (when (fboundp 'show-smartparens-mode)  ;;   (show-smartparens-mode 0)))(setq server-socket-dir "~/.emacs.d/server-dir") (require 'server)(or (server-running-p) (server-start))(add-hook 'auto-revert-mode-hook 'etc-log-tail-handler)(add-hook 'after-init-hook  (lambda ()     (global-auto-revert-mode nil)    )  );; (add-to-list 'exec-path "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Tools\\MSVC\\14.26.28801\\bin\\Hostx64\\x64\\cl.exe");; (add-to-list 'exec-path "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Tools\\MSVC\\14.26.28801\\bin\\Hostx64\\x64\\clang");; (setq company-clang-executable "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Tools\\MSVC\\14.26.28801\\bin\\Hostx64\\x64\\cl.exe");; (setq company-clang-executable "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Tools\\MSVC\\14.26.28801\\bin\\Hostx64\\x64\\clang");; (setq company-require-match nil);; (set-face-attribute 'region nil :background "dark slate blue");; (set-face-attribute 'lazy-highlight nil :background "dark cyan");; (set-face-attribute 'font-lock-constant-face nil :foreground "olive drab");; (defun casey-never-split-a-window nil);; (setq split-window-preferred-function 'casey-never-split-a-window);; (use-package smooth-scrolling);; (smooth-scrolling-mode 1);; (use-package treemacs-persp ;;treemacs-perspective if you use perspective.el vs. persp-mode;;   :after (treemacs persp-mode) ;;or perspective vs. persp-mode;;   :ensure t;;   :config (treemacs-set-scope-type 'Perspectives));;   ;; Optional, enable =visual-line-mode= for our narrow dired buffer:;;   (add-hook 'dired-toggle-mode-hook;;             (lambda () (interactive);;               (visual-line-mode 1);;               (setq-local visual-line-fringe-indicators '(nil right-curly-arrow));;               (setq-local word-wrap nil))))(custom-set-variables ;; custom-set-variables was added by Custom. ;; If you edit it by hand, you could mess it up, so be careful. ;; Your init file should contain only one such instance. ;; If there is more than one, they won't work right. '(custom-safe-themes   '("835868dcd17131ba8b9619d14c67c127aa18b90a82438c8613586331129dda63" default)) '(package-selected-packages   '(impatient-mode msvc multiple-cursors windsize preview-LaTeX latex-preview-pane texfrag company-auctex auctex adaptive-wrap idle-highlight-mode smooth-scrolling hl-todo comment-tags comment-tags-mode modern-cpp-font-lock highlight-umbers highlightn-umbers flycheck highlight-numbers elgrep doom-themes treemacs-magit treemacs-icons-dired treemacs-projectile dired-toggle dired-sidebar which-key vertico use-package spacemacs-theme pyvenv python-mode marginalia lsp-ui friendly-shell-command friendly-shell forge doom-modeline dap-mode counsel-projectile company-quickhelp company-box company-anaconda command-log-mode)))