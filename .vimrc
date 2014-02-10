set ts=4 sw=4 tw=90
" autocmd FileType c match OverLength /\%91v.\+/
autocmd FileType c call matchdelete(1)
doautoall BufReadPost
set tags=tags
