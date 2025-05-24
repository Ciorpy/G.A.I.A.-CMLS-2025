// Principal parameters
box_length = 85;
box_width = 72;
box_height = 27;
wall_thickness = 2;
tube_length = 25;
tilt_angle = 60;

// --- Sensors ---

module cylinder_photo() {
    difference(){
        cylinder(h=tube_length, r=7, center=false, $fn=100); 
        
        translate([0, 0, tube_length-3*wall_thickness])
        cylinder(h=6*wall_thickness, r=5.5, center=false, $fn=100);
    }
}

//cylinder_photo();

module cylinder_dht() {
    difference(){
        cylinder(h=tube_length, r=9, center=false, $fn=100); 
        
        translate([-7/2, -13/2, tube_length-15])
        cube([7, 13, 17]);
    }
}

//cylinder_dht();

// --- Box ---

module fulled_box() {
    cube([box_length, box_width, box_height]);
}

//fulled_box();

// --- Box with sensors ---
module fulled_totem () {
    union() {
        fulled_box();

        translate([box_length-6*wall_thickness, 3, box_height/2])
        rotate([0, tilt_angle, -90])
        cylinder_photo();

        translate([8*wall_thickness, 3, box_height/2])
        rotate([0, tilt_angle, -90])
        cylinder_dht();
    }
};

//fulled_totem();

// --- Totem with the MKR space ---
module totem_mkr () {
    difference() {
        fulled_totem();
        
        translate([wall_thickness, wall_thickness, wall_thickness])
        cube([box_length-2*wall_thickness, box_width-2*wall_thickness, box_height]);
    }
};

// --- Final totem ---
module totem_without_closing () {
    difference() {
        totem_mkr();
        
        translate([box_length-6*wall_thickness, 6, box_height/2-2])
        rotate([0, tilt_angle, -90])
        cylinder(h=25, r=4, center=false, $fn=100);
        
        translate([8*wall_thickness, 6, box_height/2-2])
        rotate([0, tilt_angle, -90])
        cylinder(h=14, r=5, center=false, $fn=100);
    }
}

//totem_without_closing();

module totem () {
    difference() {
        totem_without_closing();
        
        translate([box_length-wall_thickness-1, wall_thickness, box_height-2*wall_thickness])
        cube([2*wall_thickness, box_width-2*wall_thickness, wall_thickness]);
        
        translate([-1, wall_thickness, box_height-2*wall_thickness])
        cube([2*wall_thickness, box_width-2*wall_thickness, wall_thickness]);
    }
}
        
totem();


module cover_box () {
    union() {        
        cube([box_length+2, box_width-2*wall_thickness-0.2, wall_thickness-0.2]);
        
        translate([box_length+1, 0, 0])
        cube([wall_thickness, box_width-2*wall_thickness-0.2, 3*wall_thickness]);
    }
}

//cover_box();
