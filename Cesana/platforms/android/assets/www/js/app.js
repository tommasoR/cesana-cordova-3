(function() {
var output = PUBNUB.$('output'),
input = PUBNUB.$('input'),
button = PUBNUB.$('button'),
avatar = PUBNUB.$('avatar'),
presence = PUBNUB.$('presence');
var channel = 'cancello_client_arduino';
// Assign a random avatar in random color
// avatar.className = 'face-' + ((Math.random() * 15 + 1) >>> 0) + ' color-' + ((Math.random() * 10 + 1) >>> 0);
avatar.className = 'face-14 color-4';
var p = PUBNUB.init({
subscribe_key: 'sub-c-0f8b63e4-f56a-11e3-a672-02ee2ddab7fe',
publish_key: 'pub-c-b13eada3-8a9c-4a20-9167-79d76f4d5767',
uuid: avatar.className
});
p.subscribe({
channel : channel,
callback : function(m) {
output.innerHTML = '<p><i class="' + m.avatar + '"></i><span>' + m.text.replace( /[<>]/ig, '' ) + '</span></p>' + output.innerHTML;
//output.innerHTML = '<p><i class="face-14 color-4"></i><span>' + m.replace( /[<>]/ig, '' ) + '</span></p>' + output.innerHTML;
//output.innerHTML = '<p><i class="face-14 color-4"></i><span>' + m + '</span></p>' + output.innerHTML;
},
presence: function(m){
if(m.occupancy > 1) {
presence.textContent = m.occupancy + ' people online';
} else {
presence.textContent = 'Nobody else is online';
}
}
});
p.bind('keyup', input, function(e) {
(e.keyCode || e.charCode) === 13 && publish()
});
p.bind('click', button, publish);
function publish() {
p.publish({
channel : channel,
message : {avatar: avatar.className, text: input.value}, 
//message : input.value,
x : (input.value='')
});
}
})();
