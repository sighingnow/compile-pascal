#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "parsec.hpp"

SCENARIO( "parsec" ) {
    GIVEN( "A parser and a source string." ) {
        REQUIRE( true );

        WHEN( "The parser parse the source text." ) {
            THEN( "The parser works." ) {
                REQUIRE( true );
            }
        }
    }
}
