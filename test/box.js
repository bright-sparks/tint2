
/**
 * @unit-test-setup
 * @ignore
 */
function setup() {
  require('Common');
}

function baseline() {
}

/**
 * @see {Box}
 * @example
 */
function run($utils) {
  var win = new Window();
  win.visible = true;
  var box = new Box();
  box.borderWidth = 2;
  box.borderColor = 'red';
  box.backgroundColor = 'rgba(0,255,0,1)';
  box.borderRadius = 13;
  win.appendChild(box);
  box.left = box.right = box.top = box.bottom = 50;

  var text = new TextInput();
  text.value = "Visual check: lime green bg, 2px red border, 13px corner radius.";
  box.appendChild(text);
  text.left = '20px';
  text.top = '150px'
  text.width = 300;
  text.readonly = true;
  setTimeout(function() {
    $utils.assert(box.borderRadius == 13);
    $utils.assert(box.borderWidth == 2);
    $utils.ok();
  },2000);
}

/**
 * @unit-test-shutdown
 * @ignore
 */
function shutdown() {
}

module.exports = {
  setup:setup, 
  run:run, 
  shutdown:shutdown, 
  shell:false,
  name:"Box",
};