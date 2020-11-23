var jq = document.createElement('script');
jq.src = "https://ajax.googleapis.com/ajax/libs/jquery/3.5.1/jquery.min.js";
document.getElementsByTagName('head')[0].appendChild(jq);
// ... give time for script to load, then type (or see below for non wait option)
jQuery.noConflict();


const getAllChars = () => {
    const startChar = 65;
    const allChars = [];
    for ( let i = 0; i < 26; i++) {
        allChars.push(String.fromCharCode(startChar+i));
    }
    return allChars;
}
let allChar = getAllChars();
let summary = {}
allChar.forEach((x) => {
    summary[x] = 0;
})

// 指令总共有 588 个
let mySet = new Set()
let data = {}
jQuery('.ref_table > tbody > tr').each(function (i, el) {
    var $tds = $(this).find('td');
        let x = $tds.eq(10).text();
        let d = $tds.eq(21).text();
        if (x !== '' && x !== 'no mnemonic' && x !== 'undefined' && x!== 'Two-byte Instructions') {
            mySet.add(x);
            data[x]=d
            summary[x[0]]++;
        }
});
console.log(mySet.size);
console.log(summary);

// mySet.forEach(x => {
//     console.log(x + ":" + data[x]);
// })

// 结果
// 586
// A: 47
// B: 15
// C: 78
// D: 13
// E: 4
// F: 157
// G: 2
// H: 22
// I: 27
// J: 38
// K: 0
// L: 25
// M: 96
// N: 7
// O: 19
// P: 239
// Q: 0
// R: 45
// S: 95
// T: 8
// U: 8
// V: 12
// W: 2
// X: 21
// Y: 0
// Z: 0