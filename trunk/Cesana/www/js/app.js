var p;
var channel;
var temperatura=0;
(function () {  
var output = PUBNUB.$('output'),
input = PUBNUB.$('input'),
button = PUBNUB.$('button'),
avatar = PUBNUB.$('avatar'),
bapri =PUBNUB.$('bapri'),
presence = PUBNUB.$('presence');
channel = 'cancello_client_arduino';// 'mchat';
// Assign a random avatar in random color
// avatar.className = 'face-' + ((Math.random() * 15 + 1) >>> 0) + ' color-' + ((Math.random() * 10 + 1) >>> 0);
avatar.className = 'face-14 color-1';
p = PUBNUB.init({
//subscribe_key: 'sub-c-0f8b63e4-f56a-11e3-a672-02ee2ddab7fe',
//publish_key: 'pub-c-b13eada3-8a9c-4a20-9167-79d76f4d5767',
subscribe_key: 'sub-c-f762fb78-2724-11e4-a4df-02ee2ddab7fe',
publish_key:   'pub-c-156a6d5f-22bd-4a13-848d-b5b4d4b36695',
uuid: PUBNUB.uuid()//avatar.className
});
p.subscribe({
channel : channel,
callback : function(m) {
    if (m.avatar === 'Arduino_temperatura'){
        temperatura=m.text;
    } else {   
    var myDate = new Date();
    //var currentTime_format= currentTime.getHours()+':'+currentTime.getMinutes();
    output.innerHTML = '<p><i class="' + m.avatar + '"></i><span>' + m.text.replace( /[<>]/ig, '' ) +'<p style="font-size:10px">' + myDate.toString() +'</p></span></p>' + output.innerHTML;
    //output.innerHTML = '<p><i class="face-14 color-4"></i><span>' + m.replace( /[<>]/ig, '' ) + '</span></p>' + output.innerHTML;
    //output.innerHTML = '<p><i class="face-14 color-4"></i><span>' + m + '</span></p>' + output.innerHTML;
    }
}
,
presence: function(m){
if(m.occupancy > 1) {
//var myDate = new Date();    
presence.textContent = m.occupancy + ' dispositivi online ';//+ myDate.toString();
} else {
presence.textContent = 'Nessuno online';
}
}

});

p.bind('keyup', input, function(e) {
(e.keyCode || e.charCode) === 13 && publish()
});

p.bind('click', button, publish);
//p.bind('click', bapri, publish);

function publish(e) {
    /* gestione con il binding
     * if (e.target.id=='bapri'){
        input.value='Apri';
    }*/
    

    p.publish({
        channel : channel,
        message : {avatar: avatar.className, text: input.value}, 
        //message : input.value,
        x : (input.value='')
    });
}



})();

function scollegati(){
 p.unsubscribe({channel : channel}); 
}

function apri(){
    p.publish({
        channel : channel,
        message : {avatar: avatar.className, text: 'Apri'}
});
}
