var VERSION = '3.0';
var VERBOSE = false;
var NEW_API = true;

function verbose(content) {
  if(VERBOSE) console.log(content);
};

/******************************* Pebble Helpers *******************************/

var hasKey = function(dict, key) {
  return typeof dict.payload[key] !== 'undefined';
};

/************************************ Data ************************************/

function ajax(url, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function() {
    callback(this.responseText);
  };
  xhr.open('GET', url);
  xhr.send();
};

var getNodesOfName = function(text, name) {
  var nodes = [];
  var tag = '<' + name;
  var endTag = '</' + name + '>';

  var index = text.indexOf(tag);
  if(index < 0) {
    console.log('Unable to find XML node ' + name);
    return null;
  }

  var i = 0;
  var spool = text;
  while(spool.indexOf(name) >= 0) {
    spool = spool.substring(spool.indexOf(tag) + tag.length);
    spool = spool.substring(spool.indexOf('>') + 1 /* > */);
    
    nodes[i] = spool.substring(0, spool.indexOf(endTag));

    spool = spool.substring(spool.indexOf(endTag) + endTag.length);
    i++;
  }

  verbose('Found ' + nodes.length + ' nodes of name ' + name);
  return nodes;
};

var getAttr = function(node, name) {
  var index = node.indexOf(name);
  if(index < 0) {
    console.log('Unable to find XML attr ' + name);
    return null;
  }

  var spool = node;
  spool = spool.substring(spool.indexOf(name) + name.length + 2 /* =" */);
  return spool.substring(0, spool.indexOf('"'));
}

function sendToPebble(lineStates) {
  Pebble.sendAppMessage({
    'LineTypeBakerloo': lineStates[0], 
    'LineTypeCentral': lineStates[1],
    'LineTypeCircle': lineStates[2],
    'LineTypeDistrict': lineStates[3],
    'LineTypeHammersmithAndCity': lineStates[4],
    'LineTypeJubilee': lineStates[5],
    'LineTypeMetropolitan': lineStates[6],
    'LineTypeNorthern': lineStates[7],
    'LineTypePicadilly': lineStates[8],
    'LineTypeVictoria': lineStates[9],
    'LineTypeWaterlooAndCity': lineStates[10]
  }, function(e) {
    console.log('Sent to Pebble!');
  }, function(e) {
    console.log('Send failed!');
  });
}

function downloadNewAPI() {
  var lineStates = [];

  ajax('https://api.tfl.gov.uk/line/mode/tube/status', function(responseText) {
    console.log('Download from unified API complete!');
    verbose(responseText);

    var lines = JSON.parse(responseText);

    // Get names
    for(var i = 0; i < lines.length; i++) {
      // Yay JSON!
      lineStates[i] = lines[i].lineStatuses[0].statusSeverityDescription;
    }

    sendToPebble(lineStates);
  });
  verbose('Downloading from unified API...');
}

function downloadOldAPI() {
  var lineStates = [];

  ajax('http://cloud.tfl.gov.uk/TrackerNet/LineStatus', function(text) {
    console.log('Download complete!');

    // Get lineStates - Boo XML
    var lineStatusArray = getNodesOfName(text, 'LineStatus');
    for(var i = 0; i < lineStatusArray.length; i++) {
      lineStates[i] = getAttr(lineStatusArray[i], 'Description');
    }
    
    // Get lineStates
    sendToPebble(lineStates);
  });
  verbose('Downloading...');

};

/******************************** PebbleKit JS ********************************/

Pebble.addEventListener('ready', function(e) {
  console.log('PebbleKit JS ready! Version ' + VERSION);

  // Inform that JS is ready
  Pebble.sendAppMessage({'AppMessageKeyJSReady': 1}, function(e) {
    verbose('Send ready event successfully');
  }, function(e) {
    console.log('Failed to send ready event!');
  });
});

Pebble.addEventListener('appmessage', function(dict) {
  if(hasKey(dict, 'AppMessageKeyJSReady')) {
    verbose('Data request receieved.');
    if(NEW_API) {
      downloadNewAPI();
    } else {
      downloadOldAPI();
    }
  } else {
    console.log('Unexpected message: ' + JSON.stringify(data.payload));
  }
});
