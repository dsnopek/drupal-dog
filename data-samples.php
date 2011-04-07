<?php
// sketching out the data tracked in a makefile

// global settings
$global = new stdClass();
$global->cache = 'path/to/cache/repository';


// main superrepo itself, a clone of core
$core = new stdClass();
$core->cloneurl = 'git://git.drupal.org/project/drupal.git'; // use vanilla core
$core->version = '7.x'; // use the 7.x branch


// for a contained project from d.o, a small module (bad_judgement) in this case
$project = new stdClass();
$project->type = 'module';
$project->cloneurl = 'git://git.drupal.org/project/bad_judgement.git';
$project->hookset = 'name-of-hook-set'; // name of a set of hooks drush should attach to this repository
